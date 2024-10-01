/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_scs_helper.hpp"

#include "sc_debug.hpp"
#include "sc_link.hpp"
#include "sc_memory.hpp"

#include "sc_keynodes.hpp"

#include "scs/scs_parser.hpp"

#include <regex>
#include <utility>

namespace impl
{
class StructGenerator
{
  friend class ::SCsHelper;

protected:
  StructGenerator(ScMemoryContext & ctx, SCsFileInterfacePtr fileInterface, ScAddr const & outputStructure)
    : m_ctx(ctx)
    , m_fileInterface(std::move(fileInterface))
    , m_outputStructure(outputStructure)
  {
  }

  void operator()(scs::Parser const & parser)
  {
    // generate aliases
    auto const & aliases = parser.GetAliases();
    for (auto const & it : aliases)
    {
      auto const & parsedElement = parser.GetParsedElement(it.second);
      if (parsedElement.GetType().IsNode())
        ResolveElement(parsedElement);
    }

    // generate triples
    auto const & triples = parser.GetParsedTriples();
    for (auto const & t : triples)
    {
      auto const & src = parser.GetParsedElement(t.m_source);
      auto const & connector = parser.GetParsedElement(t.m_connector);
      auto const & trg = parser.GetParsedElement(t.m_target);

      auto const & srcAddrResult = ResolveElement(src);
      auto const & trgAddrResult = ResolveElement(trg);

      if (!connector.GetType().IsConnector())
        SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Specified sc-connector in triple has incorrect type.");

      ScAddr const arcAddr = m_ctx.GenerateConnector(connector.GetType(), srcAddrResult.first, trgAddrResult.first);
      m_idtfCache.insert({connector.GetIdtf(), arcAddr});

      if (m_outputStructure.IsValid())
      {
        AppendToOutputStructure(srcAddrResult.first, arcAddr, trgAddrResult.first);
        AppendToOutputStructure(srcAddrResult.second);
        AppendToOutputStructure(trgAddrResult.second);
      }
    }

    parser.ForEachParsedElement(
        [this](scs::ParsedElement const & el)
        {
          if (m_idtfCache.find(el.GetIdtf()) == m_idtfCache.end() && !el.GetType().IsConnector()
              && !scs::TypeResolver::IsKeynodeType(el.GetIdtf()))
            ResolveElement(el);
        });
  }

private:
  template <class... Args>
  void AppendToOutputStructure(Args const &... addrs)
  {
    ScAddrVector const & addrVector{addrs...};
    for (ScAddr const & addr : addrVector)
    {
      if (!m_ctx.CheckConnector(m_outputStructure, addr, ScType::ConstPermPosArc))
        m_ctx.GenerateConnector(ScType::ConstPermPosArc, m_outputStructure, addr);
    }
  }

  ScAddrVector SetSCsGlobalIdtf(std::string const & idtf, ScAddr const & addr)
  {
    // Generate construction manually. To avoid recursive call of ScMemoryContextEventsPendingGuard
    ScAddr const linkAddr = m_ctx.GenerateLink();
    ScLink link(m_ctx, linkAddr);
    link.Set(idtf);

    ScAddr const arcAddr = m_ctx.GenerateConnector(ScType::ConstCommonArc, addr, linkAddr);
    ScAddr const relAddr = m_ctx.GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_scs_global_idtf, arcAddr);

    return {linkAddr, arcAddr, relAddr};
  }

  ScAddr FindBySCsGlobalIdtf(std::string const & idtf) const
  {
    ScAddr result;

    auto const links = m_ctx.SearchLinksByContent(idtf);
    for (ScAddr const & addr : links)
    {
      ScTemplate templ;

      templ.Quintuple(
          ScType::Unknown >> "_el",
          ScType::VarCommonArc,
          addr,
          ScType::VarPermPosArc,
          ScKeynodes::nrel_scs_global_idtf);

      ScTemplateSearchResult searchResult;
      if (m_ctx.SearchByTemplate(templ, searchResult))
      {
        if (result.IsValid() || searchResult.Size() > 1)
          SC_THROW_EXCEPTION(
              utils::ExceptionInvalidState, "There are more then 1 element with global identifier: " << idtf);

        result = searchResult[0]["_el"];
      }
    }

    return result;
  }

  std::pair<ScAddr, ScAddrVector> ResolveElement(scs::ParsedElement const & el)
  {
    ScAddrVector result;
    ScAddr resultAddr;
    std::string const & idtf = el.GetIdtf();
    auto const it = m_idtfCache.find(idtf);
    if (it != m_idtfCache.cend())
      resultAddr = it->second;
    else
    {
      // try to find existing
      if (el.GetVisibility() == scs::Visibility::System)
      {
        ScSystemIdentifierQuintuple quintuple;
        m_ctx.SearchElementBySystemIdentifier(el.GetIdtf(), quintuple);
        resultAddr = quintuple.addr1;
        result = {quintuple.addr2, quintuple.addr3, quintuple.addr4, quintuple.addr5};
      }
      else if (el.GetVisibility() == scs::Visibility::Global)
        resultAddr = FindBySCsGlobalIdtf(el.GetIdtf());

      // generate new one
      if (!resultAddr.IsValid())
      {
        ScType const & type = el.GetType();
        if (type.IsNode())
        {
          if (type.IsLink())
          {
            resultAddr = m_ctx.GenerateLink(type);
            SetupLinkContent(resultAddr, el);
          }
          else
            resultAddr = m_ctx.GenerateNode(type);
        }
        else
          SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Incorrect element type at this state.");

        // setup system identifier
        if (el.GetVisibility() == scs::Visibility::System)
        {
          ScSystemIdentifierQuintuple quintuple;
          m_ctx.SetElementSystemIdentifier(el.GetIdtf(), resultAddr, quintuple);
          result = {quintuple.addr2, quintuple.addr3, quintuple.addr4, quintuple.addr5};
        }
        else if (el.GetVisibility() == scs::Visibility::Global)
        {
          result = SetSCsGlobalIdtf(el.GetIdtf(), resultAddr);
        }
      }
      else
      {
        ScType const & newType = el.GetType();
        ScType const & oldType = m_ctx.GetElementType(resultAddr);
        if (newType != oldType)
        {
          if (oldType.CanExtendTo(newType))
            m_ctx.SetElementSubtype(resultAddr, newType);
          else if (!newType.CanExtendTo(oldType))
            SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Duplicate element type for " + el.GetIdtf());
        }
      }

      // anyway save in cache
      m_idtfCache.insert(std::make_pair(idtf, resultAddr));
    }

    return {resultAddr, result};
  }

  template <typename T>
  bool SetLinkContentT(ScAddr const & linkAddr, std::string const & value)
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
      // check if it's a number format
      std::regex const rNumber(
          "^\\^\"(int8|int16|int32|int64|uint8|uint16|uint32|uint64|float|double)\\s*:\\s*([0-9]+|[0-9]+[.][0-9]+)\"$");
      std::smatch result;
      if (std::regex_match(el.GetValue(), result, rNumber))
      {
        SC_ASSERT(result.size() == 3, "Invalid link content format");

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
          SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Unsupported link binary type: " + type);

        if (!result)
          SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't parse value from: " + el.GetValue());
      }
      else
      {
        ScLink link(m_ctx, linkAddr);
        link.Set(el.GetValue());
      }
    }
  }

private:
  ScMemoryContext & m_ctx;
  SCsFileInterfacePtr m_fileInterface;
  ScAddr m_outputStructure;

  std::unordered_map<std::string, ScAddr> m_idtfCache;
};

}  // namespace impl

SCsHelper::SCsHelper(ScMemoryContext & ctx, SCsFileInterfacePtr fileInterface)
  : m_ctx(ctx)
  , m_fileInterface(std::move(fileInterface))
{
}

bool SCsHelper::GenerateBySCsText(std::string const & scsText, ScAddr const & outputStructure)
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
      impl::StructGenerator generate(m_ctx, m_fileInterface, outputStructure);
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

void SCsHelper::GenerateBySCsTextLazy(std::string const & scsText, ScAddr const & outputStructure)
{
  ScMemoryContextEventsPendingGuard guard(m_ctx);

  scs::Parser parser;
  if (!parser.Parse(scsText))
    SC_THROW_EXCEPTION(utils::ExceptionParseError, parser.GetParseError());
  else
  {
    impl::StructGenerator generate(m_ctx, m_fileInterface, outputStructure);
    generate(parser);
  }
}

std::string const & SCsHelper::GetLastError() const
{
  return m_lastError;
}
