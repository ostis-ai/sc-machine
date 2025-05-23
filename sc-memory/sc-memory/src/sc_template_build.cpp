/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_template.hpp"

#include <vector>
#include <unordered_map>
#include <set>
#include <iostream>
#include <sstream>

#include "sc-memory/sc_memory.hpp"

namespace
{

class TemplateObjectInfo
{
public:
  TemplateObjectInfo(ScAddr const & addr, ScType const & type, std::string idtf)
    : m_addr(addr)
    , m_type(type)
    , m_idtf(std::move(idtf))
    , m_sourceHash(0)
    , m_targetHash(0)
  {
  }

  [[nodiscard]] inline bool IsConnector() const
  {
    return m_type.IsConnector();
  }

  [[nodiscard]] inline ScAddr const & GetAddr() const
  {
    return m_addr;
  }

  [[nodiscard]] inline std::string const & GetIdentifier() const
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

  inline void SetSourceHash(ScAddr::HashType const & sourceHash)
  {
    m_sourceHash = sourceHash;
  }

  inline void SetTargetHash(ScAddr::HashType const & targetHash)
  {
    m_targetHash = targetHash;
  }

  [[nodiscard]] inline ScAddr::HashType GetSourceHash() const
  {
    return m_sourceHash;
  }

  [[nodiscard]] inline ScAddr::HashType GetTargetHash() const
  {
    return m_targetHash;
  }

private:
  ScAddr m_addr;
  ScType m_type;
  std::string m_idtf;

  ScAddr::HashType m_sourceHash;
  ScAddr::HashType m_targetHash;
};

}  // namespace

class ScTemplateBuilder
{
  friend class ScTemplate;
  using ConnectorDependencyMap = std::unordered_multimap<ScAddr::HashType, ScAddr::HashType>;
  using ObjectHashToInfoMap = std::unordered_map<ScAddr::HashType, TemplateObjectInfo>;
  using HashSet = std::set<ScAddr::HashType>;
  using LevelCache = std::unordered_map<ScAddr::HashType, size_t>;

protected:
  ScTemplateBuilder(ScAddr const & templateAddr, ScMemoryContext & context, ScTemplateParams const & params)
    : m_templateAddr(templateAddr)
    , m_context(context)
  {
    auto const & replacements = params.GetAll();
    for (auto const & item : replacements)
    {
      ScAddr const & addr = m_context.SearchElementBySystemIdentifier(item.first);
      if (m_context.IsElement(addr))
      {
        TemplateObjectInfo objInfo = CollectObjectInfo(item.second, addr);
        m_objectInfos.insert({addr.Hash(), objInfo});
      }
      else
      {
        TemplateObjectInfo objInfo = CollectObjectInfo(item.second, item.second);
        std::stringstream ss(item.first);
        ScAddr::HashType hash;
        ss >> hash;
        ScAddr varNode(hash);
        m_objectInfos.insert({varNode.Hash(), objInfo});
      }
    }
  }

  void operator()(ScTemplate * targetTemplate)
  {
    if (!targetTemplate)
      return;

    HashSet independentConnectorHashes;

    ScIterator3Ptr iterator = m_context.CreateIterator3(m_templateAddr, ScType::ConstPermPosArc, ScType::Unknown);
    while (iterator->Next())
    {
      ScAddr const & objectAddr = iterator->Get(2);

      auto const & it = m_objectInfos.find(objectAddr.Hash());
      TemplateObjectInfo objInfo = it == m_objectInfos.cend() ? CollectObjectInfo(objectAddr) : it->second;

      if (objInfo.IsConnector())
      {
        auto [sourceAddr, targetAddr] = m_context.GetConnectorIncidentElements(objectAddr);
        objInfo.SetSourceHash(sourceAddr.Hash());
        objInfo.SetTargetHash(targetAddr.Hash());

        ScType const sourceType = m_context.GetElementType(sourceAddr);
        ScType const targetType = m_context.GetElementType(targetAddr);
        if (!sourceType.IsConnector() && !targetType.IsConnector())
          independentConnectorHashes.insert(objInfo.GetHash());
        if (sourceType.IsConnector())
          m_connectorDependencyMap.insert({objInfo.GetHash(), sourceAddr.Hash()});
        if (targetType.IsConnector())
          m_connectorDependencyMap.insert({objInfo.GetHash(), targetAddr.Hash()});
      }

      m_objectInfos.try_emplace(objInfo.GetHash(), std::move(objInfo));
    }

    std::vector<HashSet> connectorsByDependencyLevel;
    connectorsByDependencyLevel.emplace_back(independentConnectorHashes);
    GroupConnectorsByDependencyLevel(connectorsByDependencyLevel);

    for (auto const & connectorsWithSameLevel : connectorsByDependencyLevel)
    {
      for (ScAddr::HashType const & connectorHash : connectorsWithSameLevel)
      {
        TemplateObjectInfo const & connectorInfo = m_objectInfos.at(connectorHash);
        TemplateObjectInfo const & sourceInfo = m_objectInfos.at(connectorInfo.GetSourceHash());
        TemplateObjectInfo const & targetInfo = m_objectInfos.at(connectorInfo.GetTargetHash());

        auto const & param = [targetTemplate](TemplateObjectInfo const & objInfo) -> ScTemplateItem
        {
          return objInfo.GetType().IsConst() ? objInfo.GetAddr() >> objInfo.GetIdentifier()
                                             : (targetTemplate->HasReplacement(objInfo.GetIdentifier())
                                                    ? objInfo.GetIdentifier()
                                                    : objInfo.GetType() >> objInfo.GetIdentifier());
        };

        targetTemplate->Triple(param(sourceInfo), param(connectorInfo), param(targetInfo));
      }
    }
  }

protected:
  ScAddr m_templateAddr;
  ScMemoryContext & m_context;
  ScTemplateParams m_params;

  ObjectHashToInfoMap m_objectInfos;
  ConnectorDependencyMap m_connectorDependencyMap;

private:
  TemplateObjectInfo CollectObjectInfo(ScAddr const & objAddr, std::string objIdtf = "") const
  {
    ScType const objType = m_context.GetElementType(objAddr);
    if (objIdtf.empty())
      objIdtf = std::string(objAddr);
    return {objAddr, objType, objIdtf};
  }

  void GroupConnectorsByDependencyLevel(std::vector<HashSet> & connectorsByDependencyLevel)
  {
    LevelCache dependencyLevelCache;

    for (auto const & dependencyPair : m_connectorDependencyMap)
    {
      size_t level = GetConnectorDependencyLevel(dependencyPair.first, dependencyPair.second, dependencyLevelCache);
      if (level >= connectorsByDependencyLevel.size())
        connectorsByDependencyLevel.resize(level + 1);

      connectorsByDependencyLevel.at(level).insert(dependencyPair.first);
    }
  }

  size_t GetConnectorDependencyLevel(
      ScAddr::HashType const & connectorHash,
      ScAddr::HashType const & dependentConnectorHash,
      LevelCache & levelCache) const
  {
    auto it = levelCache.find(connectorHash);
    if (it != levelCache.end())
      return it->second;

    size_t maxLevel = 0;
    UpdateConnectorDependencyLevel(connectorHash, dependentConnectorHash, maxLevel, 1, levelCache);
    levelCache[connectorHash] = maxLevel;
    return maxLevel;
  }

  size_t UpdateConnectorDependencyLevel(
      ScAddr::HashType const & connectorHash,
      ScAddr::HashType const & dependentConnectorHash,
      size_t & maxLevel,
      size_t currentLevel,
      LevelCache & levelCache) const
  {
    auto const range = m_connectorDependencyMap.equal_range(connectorHash);
    for (auto it = range.first; it != range.second; ++it)
    {
      size_t nextLevel = currentLevel + 1;
      size_t subLevel = GetConnectorDependencyLevel(it->second, dependentConnectorHash, levelCache);

      if (subLevel == nextLevel && currentLevel > maxLevel)
        maxLevel = currentLevel;
    }
    return currentLevel;
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
