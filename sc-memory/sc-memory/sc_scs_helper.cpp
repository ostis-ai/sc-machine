/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_scs_helper.hpp"

#include "sc_debug.hpp"
#include "sc_link.hpp"
#include "sc_memory.hpp"

#include "scs/scs_parser.hpp"

#include <regex>

namespace impl
{

class StructGenerator
{
  friend class ::SCsHelper;

protected:
  StructGenerator(ScMemoryContext & ctx, SCsFileInterfacePtr const & fileInterface, bool isAddToRoot)
    : m_fileInterface(fileInterface)
    , m_ctx(ctx)
  {
    m_kNrelSCsGlobalIdtf = m_ctx.HelperResolveSystemIdtf("nrel_scs_global_idtf", ScType::NodeConstNoRole);
    SC_ASSERT(m_kNrelSCsGlobalIdtf.IsValid(), ());
    this->isAddToRoot=isAddToRoot;
    this->concertedKB = m_ctx.HelperFindBySystemIdtf(concertedKBName);
  }

  void operator() (scs::Parser const & parser)
  {
    // generate aliases
    auto const & aliases = parser.GetAliases();
    for (auto const & it : aliases)
      ResolveElement(parser.GetParsedElement(it.second));

    // generate triples
    auto const & triples = parser.GetParsedTriples();
    for (auto const & t : triples)
    {
      auto const & src = parser.GetParsedElement(t.m_source);
      auto const & edge = parser.GetParsedElement(t.m_edge);
      auto const & trg = parser.GetParsedElement(t.m_target);

//        if (src.GetType().IsEdge() | trg.GetType().IsEdge()) {
//            continue;
//        }

      ScAddr const srcAddr = ResolveElement(src);
      ScAddr const trgAddr = ResolveElement(trg);

      SC_ASSERT(srcAddr.IsValid() && trgAddr.IsValid(), ());
      if (!edge.GetType().IsEdge()) {
          SC_THROW_EXCEPTION(utils::ExceptionInvalidType,
                             "Edge in triple has incorrect type");
      }

      ScAddr const edgeAddr = m_ctx.CreateEdge(edge.GetType(), srcAddr, trgAddr);
      if (isAddToRoot) {
          m_ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, this->concertedKB, edgeAddr);
      }
      SC_ASSERT(edgeAddr.IsValid(), ());
      m_idtfCache.insert(std::make_pair(edge.GetIdtf(), edgeAddr));
    }

    parser.ForEachParsedElement([this](scs::ParsedElement const & el)
    {
      if (m_idtfCache.find(el.GetIdtf()) == m_idtfCache.end() &&
          !el.GetType().IsEdge() &&
          !scs::TypeResolver::IsKeynodeType(el.GetIdtf()))
      {
        ResolveElement(el);
      }
    });
  }

private:

  void SetSCsGlobalIdtf(std::string const & idtf, ScAddr const & addr)
  {
    SC_ASSERT(m_kNrelSCsGlobalIdtf.IsValid(), ());

    // Generate construction manually. To avoid recursive call of ScMemoryContextEventsPendingGuard

    ScAddr const linkAddr = m_ctx.CreateLink();
    if (isAddToRoot) {
        m_ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, this->concertedKB, linkAddr);
    }
    ScLink link(m_ctx, linkAddr);
    link.Set(idtf);


    ScAddr const edgeAddr = m_ctx.CreateEdge(ScType::EdgeDCommonConst, addr, linkAddr);
      if (isAddToRoot) {
          m_ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, this->concertedKB, edgeAddr);
      }
    ScAddr const edgeAddr2 = m_ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, m_kNrelSCsGlobalIdtf, edgeAddr);
      if (isAddToRoot) {
          m_ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, this->concertedKB, edgeAddr2);
      }
  }

  ScAddr FindBySCsGlobalIdtf(std::string const & idtf) const
  {
    SC_ASSERT(m_kNrelSCsGlobalIdtf.IsValid(), ());

    ScAddr result;

    auto const links = m_ctx.FindLinksByContent(idtf);
    for (ScAddr const & addr : links)
    {
      ScTemplate templ;

      templ.TripleWithRelation(
        ScType::Unknown >> "_el",
        ScType::EdgeDCommonVar,
        addr,
        ScType::EdgeAccessVarPosPerm,
        m_kNrelSCsGlobalIdtf);

      ScTemplateSearchResult searchResult;
      if (m_ctx.HelperSearchTemplate(templ, searchResult))
      {
        if (result.IsValid() || searchResult.Size() > 1)
        {
          SC_THROW_EXCEPTION(utils::ExceptionInvalidState,
                             "There are more then 1 element with global identifier: " << idtf);
        }

        result = searchResult[0]["_el"];
      }
    }

    return result;
  }

  ScAddr ResolveElement(scs::ParsedElement const & el)
  {
    ScAddr result;
    std::string const & idtf = el.GetIdtf();
    auto const it = m_idtfCache.find(idtf);
    if (it != m_idtfCache.end())
    {
      result = it->second;
    }
    else
    {
      // try to find existing
      if (el.GetVisibility() == scs::Visibility::System)
      {
        result = m_ctx.HelperFindBySystemIdtf(el.GetIdtf());
      }
      else if (el.GetVisibility() == scs::Visibility::Global)
      {
        result = FindBySCsGlobalIdtf(el.GetIdtf());
      }

      if (isAddToRoot) {
          if (el.GetIdtf() == "..edge_0") {
              int x=1;
          }
      }

      // create new one
      if (!result.IsValid())
      {
        ScType const & type = el.GetType();
        if (type.IsNode())
        {
          result = m_ctx.CreateNode(type);
            if (isAddToRoot) {
                m_ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, this->concertedKB, result);
            }
        }
        else if (type.IsLink())
        {
          result = m_ctx.CreateLink(type);
            if (isAddToRoot) {
                m_ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, this->concertedKB, result);
            }
          SetupLinkContent(result, el);
        }
        else {
          SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Incorrect element type at this state");
        }

        // setup system identifier
        if (el.GetVisibility() == scs::Visibility::System)
        {
          m_ctx.HelperSetSystemIdtf(el.GetIdtf(), result);
          if (isAddToRoot) {
              ScAddr sysId = m_ctx.HelperFindBySystemIdtf("nrel_system_identifier");
              ScIterator5Ptr it = m_ctx.Iterator5(result,
                                                   ScType::EdgeDCommonConst,
                                                   ScType::Const,
                                                   ScType::EdgeAccessConstPosPerm,
                                                   sysId);
              if (it->Next()) {
                  ScAddr obj = it->Get(1);
                  m_ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, this->concertedKB, obj);
                  obj = it->Get(2);
                  m_ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, this->concertedKB, obj);
                  obj = it->Get(3);
                  m_ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, this->concertedKB, obj);
              }
          }
        }
        else if (el.GetVisibility() == scs::Visibility::Global)
        {
          SetSCsGlobalIdtf(el.GetIdtf(), result);
        }
        
      }

      SC_ASSERT(result.IsValid(), ());

      // anyway save in cache
      m_idtfCache.insert(std::make_pair(idtf, result));
    }

    return result;
  }

  template <typename T> bool SetLinkContentT(ScAddr const & linkAddr, std::string const & value)
  {
    T number;
    auto const result = utils::StringUtils::ParseNumber<T>(value, number);
    if (result)
    {
      ScLink link(m_ctx, linkAddr);
      link.Set(number);

      return true;
    }

    return false;
  }

  void SetupLinkContent(ScAddr const & linkAddr, scs::ParsedElement const & el)
  {
    if (el.IsURL())
    {
      ScStreamPtr stream = m_fileInterface->GetFileContent(el.GetValue());
      if (stream)
      {
        ScLink link(m_ctx, linkAddr);
        link.Set(stream);
      }
    }
    else
    {
      // chekc if it's a number format
      std::regex const rNumber("^\\^\"(int8|int16|int32|int64|uint8|uint16|uint32|uint64|float|double)\\s*:\\s*([0-9]+|[0-9]+[.][0-9]+)\"$");
      std::smatch result;
      if (std::regex_match(el.GetValue(), result, rNumber))
      {
        SC_ASSERT(result.size() == 3, ());

        std::string const type = result[1];
        std::string const value = result[2];

        bool result = true;
        if (type == "float")
          result = SetLinkContentT<float>(linkAddr, value);
        else if (type == "double")
          result = SetLinkContentT<double>(linkAddr, value);
        else if (type == "int8")
          result = SetLinkContentT<int8_t>(linkAddr, value);
        else if (type == "int16")
          result = SetLinkContentT<int16_t>(linkAddr, value);
        else if (type == "int32")
          result = SetLinkContentT<int32_t>(linkAddr, value);
        else if (type == "int64")
          result = SetLinkContentT<int64_t>(linkAddr, value);
        else if (type == "uint8")
          result = SetLinkContentT<uint8_t>(linkAddr, value);
        else if (type == "uint16")
          result = SetLinkContentT<uint16_t>(linkAddr, value);
        else if (type == "uint32")
          result = SetLinkContentT<uint32_t>(linkAddr, value);
        else if (type == "uint64")
          result = SetLinkContentT<uint64_t>(linkAddr, value);
        else
        {
          SC_THROW_EXCEPTION(utils::ExceptionInvalidType,
                             "Unsupported link binary type: " + type);
        }

        if (!result)
        {
          SC_THROW_EXCEPTION(utils::ExceptionInvalidState,
                             "Can't parse value from: " + el.GetValue());
        }
      }
      else
      {
        ScLink link(m_ctx, linkAddr);
        link.Set(el.GetValue());
      }
    }
  }

private:
  SCsFileInterfacePtr m_fileInterface;
  ScMemoryContext & m_ctx;

  std::unordered_map<std::string, ScAddr> m_idtfCache;
  ScAddr m_kNrelSCsGlobalIdtf;
  ScAddr concertedKB;
  std::string concertedKBName = "concertedKB_hash_iF95K2";
  std::string concertedPartSetName = "concerted_part_of_kb";
  bool isAddToRoot;
};

} // namespace impl

SCsHelper::SCsHelper(ScMemoryContext & ctx, SCsFileInterfacePtr const & fileInterface)
  : m_ctx(ctx)
  , m_fileInterface(fileInterface)
{
}

bool SCsHelper::GenerateBySCsText(std::string const & scsText, bool isAddToRoot)
{
  m_lastError = "";
  bool result = true;

  ScMemoryContextEventsPendingGuard guard(m_ctx);

  scs::Parser parser;
  try
  {
    if (!parser.Parse(scsText))
    {
      m_lastError = parser.GetParseError();
      result = false;
    }
    else
    {
      impl::StructGenerator generate(m_ctx, m_fileInterface, isAddToRoot);
      generate(parser);
    }
  }
  catch (utils::ScException const & ex)
  {
    m_lastError = ex.Description();
    result = false;
  }

  return result;
}

std::string const & SCsHelper::GetLastError() const
{
  return m_lastError;
}
