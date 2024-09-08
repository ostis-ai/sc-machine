/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"

#include <vector>
#include <unordered_map>
#include <iostream>

#include "sc_memory.hpp"

namespace
{
class ObjectInfo
{
public:
  ObjectInfo(ScAddr const & addr, ScType const & type, std::string idtf)
    : m_addr(addr)
    , m_type(type)
    , m_idtf(std::move(idtf))
    , m_srcHash(0)
    , m_trgHash(0)
  {
  }

  [[nodiscard]] inline bool IsEdge() const
  {
    return m_type.IsEdge();
  }

  [[nodiscard]] inline ScAddr const & GetAddr() const
  {
    return m_addr;
  }

  [[nodiscard]] inline std::string const & GetIdtf() const
  {
    return m_idtf;
  }

  [[nodiscard]] inline ScType const & GetType() const
  {
    return m_type;
  }

  [[nodiscard]] inline ScAddr::HashType GetHash() const
  {
    return m_addr.Hash();
  }

  inline void SetSourceHash(ScAddr::HashType const & srcHash)
  {
    m_srcHash = srcHash;
  }

  inline void SetTargetHash(ScAddr::HashType const & trgHash)
  {
    m_trgHash = trgHash;
  }

  [[nodiscard]] inline ScAddr::HashType const & GetSourceHash() const
  {
    return m_srcHash;
  }

  [[nodiscard]] inline ScAddr::HashType const & GetTargetHash() const
  {
    return m_trgHash;
  }

private:
  ScAddr m_addr;
  ScType m_type;
  std::string m_idtf;

  ScAddr::HashType m_srcHash;
  ScAddr::HashType m_trgHash;
};

}  // namespace

class ScTemplateBuilder
{
  friend class ScTemplate;
  using ConnectorDependenceMap = std::unordered_multimap<ScAddr::HashType, ScAddr::HashType>;
  using ObjectToIdtfMap = std::unordered_map<ScAddr::HashType, ObjectInfo>;
  using ScAddrHashSet = std::set<ScAddr::HashType>;

protected:
  ScTemplateBuilder(ScAddr const & translatableTemplateAddr, ScMemoryContext & ctx, ScTemplateParams const & params)
    : m_templateAddr(translatableTemplateAddr)
    , m_context(ctx)
  {
    auto const & replacements = params.GetAll();
    for (auto const & item : replacements)
    {
      ScAddr const & addr = m_context.SearchElementBySystemIdentifier(item.first);
      if (m_context.IsElement(addr))
      {
        ObjectInfo obj = CollectObjectInfo(item.second, std::to_string(addr.Hash()));
        m_elements.insert({addr.Hash(), obj});
      }
      else
      {
        ObjectInfo obj = CollectObjectInfo(item.second, std::to_string(item.second.Hash()));
        std::stringstream ss(item.first);
        sc_addr_hash hash;
        ss >> hash;
        ScAddr const & varNode = ScAddr(hash);
        m_elements.insert({varNode.Hash(), obj});
      }
    }
  }

  void operator()(ScTemplate * inTemplate)
  {
    // TODO: Add blocking sc-structure
    ScAddrHashSet independentConnectors;

    // define connectors set and independent connectors set
    ScIterator3Ptr iter = m_context.Iterator3(m_templateAddr, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
    while (iter->Next())
    {
      ScAddr const & objAddr = iter->Get(2);

      auto const & it = m_elements.find(objAddr.Hash());
      ObjectInfo obj = it == m_elements.cend() ? CollectObjectInfo(objAddr) : it->second;

      if (obj.IsEdge())
      {
        auto [objSrc, objTrg] = m_context.GetConnectorIncidentElements(objAddr);
        obj.SetSourceHash(objSrc.Hash());
        obj.SetTargetHash(objTrg.Hash());

        ScType const srcType = m_context.GetElementType(objSrc), trgType = m_context.GetElementType(objTrg);
        if (!srcType.IsEdge() && !trgType.IsEdge())
          independentConnectors.insert(obj.GetHash());
        if (srcType.IsEdge())
          m_connectorDependenceMap.insert({obj.GetHash(), objSrc.Hash()});
        if (trgType.IsEdge())
          m_connectorDependenceMap.insert({obj.GetHash(), objTrg.Hash()});
      }

      m_elements.insert({obj.GetHash(), obj});
    }

    // split connectors set by their power
    std::vector<ScAddrHashSet> powerDependentConnectors;
    powerDependentConnectors.emplace_back(independentConnectors);
    SplitConnectorsByDependencePower(powerDependentConnectors);

    for (auto const & equalDependentConnectors : powerDependentConnectors)
    {
      for (ScAddr::HashType const & connectorHash : equalDependentConnectors)
      {
        ObjectInfo const & connector = m_elements.at(connectorHash);
        ObjectInfo const & src = m_elements.at(connector.GetSourceHash());
        ObjectInfo const & trg = m_elements.at(connector.GetTargetHash());

        auto const & param = [&inTemplate](ObjectInfo const & obj) -> ScTemplateItem
        {
          return obj.GetType().IsConst()
                     ? obj.GetAddr() >> obj.GetIdtf()
                     : (inTemplate->HasReplacement(obj.GetIdtf()) ? obj.GetIdtf() : obj.GetType() >> obj.GetIdtf());
        };

        inTemplate->Triple(param(src), param(connector), param(trg));
      }
    }
  }

protected:
  ScAddr m_templateAddr;
  ScMemoryContext & m_context;
  ScTemplateParams m_params;

  // all objects in template
  ObjectToIdtfMap m_elements;
  ConnectorDependenceMap m_connectorDependenceMap;

private:
  ObjectInfo CollectObjectInfo(ScAddr const & objAddr, std::string objIdtf = "") const
  {
    ScType const objType = m_context.GetElementType(objAddr);
    if (objIdtf.empty())
      objIdtf = std::to_string(objAddr.Hash());

    return {objAddr, objType, objIdtf};
  }

  void SplitConnectorsByDependencePower(std::vector<ScAddrHashSet> & powerDependentConnectors)
  {
    for (auto const & hPair : m_connectorDependenceMap)
    {
      size_t const power = GetConnectorDependencePower(hPair.first, hPair.second);
      size_t size = powerDependentConnectors.size();

      if (power >= size)
      {
        while (size <= power)
        {
          powerDependentConnectors.emplace_back();
          ++size;
        }
      }

      powerDependentConnectors.at(power).insert(hPair.first);
    }
  }

  // get connector dependence power from other connector
  inline size_t GetConnectorDependencePower(
      ScAddr::HashType const & connectorHash,
      ScAddr::HashType const & otherConnectorHash) const
  {
    size_t max = 0;
    DefineConnectorDependencePower(connectorHash, otherConnectorHash, max, 1);
    return max;
  }

  // count connector dependence power from other connector
  inline size_t DefineConnectorDependencePower(
      ScAddr::HashType const & connectorHash,
      ScAddr::HashType const & otherConnectorHash,
      size_t & max,
      size_t power) const
  {
    auto const range = m_connectorDependenceMap.equal_range(connectorHash);
    for (auto it = range.first; it != range.second; ++it)
    {
      size_t incPower = power + 1;

      if (DefineConnectorDependencePower(it->second, otherConnectorHash, max, incPower) == incPower && power > max)
        max = power;
    }
    return power;
  }
};

void ScTemplate::TranslateFrom(
    ScMemoryContext & ctx,
    ScAddr const & translatableTemplateAddr,
    ScTemplateParams const & params)
{
  ScTemplateBuilder builder(translatableTemplateAddr, ctx, params);
  builder(this);
}
