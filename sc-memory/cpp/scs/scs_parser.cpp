/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "scs_parser.hpp"

#include "../sc_memory.hpp"
#include "../sc_debug.hpp"

#include <limits>

#if SC_PLATFORM_WIN32
#endif

#if SC_DEBUG_MODE
/// TODO: @deniskoronchik: make as cmake parameter
#   define SCS_DEBUG_TOKEN 1
#endif

#if SCS_DEBUG_TOKEN
#   define DEBUG_TOKEN(__msg) { std::cout << __msg << std::endl; }
#else
#   define DEBUG_TOKEN(__msg)
#endif

#include "scsLexer.hpp"
#include "scsParser.hpp"

#include <iostream>

namespace
{

bool IsLevel1Idtf(std::string const & idtf)
{
  return (idtf.find('#') != std::string::npos);
}

std::pair<ScType, std::string> ParseIdtfLevel1(std::string const & idtf)
{
  SC_ASSERT(IsLevel1Idtf(idtf), ("You can parse just level1 identifiers"));

  size_t const n = idtf.find('#');
  if (n != std::string::npos)
  {
    std::string const typeAlias = idtf.substr(0, n);
    std::string const idtfAlias = idtf.substr(n + 1);

    ScType type = scs::TypeResolver::GetKeynodeType(typeAlias);
    if (scs::TypeResolver::IsConst(idtfAlias))
    {
      type |= ScType::Const;
    }
    else
    {
      type |= ScType::Var;
    }

    return std::make_pair(type, idtfAlias);
  }

  static const std::pair<ScType, std::string> empty = std::make_pair(ScType::Unknown, "");
  return empty;
}

}

namespace scs
{

ParsedElement::ParsedElement(std::string const & idtf, ScType const & type, bool isReversed, std::string const & value)
  : m_idtf(idtf)
  , m_type(type)
  , m_visibility(Visibility::System)
  , m_isReversed(isReversed)
  , m_value(value)
{
  if (!m_value.empty())
  {
    SC_ASSERT(m_type.IsNode() || m_type.IsLink(), ("This constructor should be used for contour/link elements"));
    m_visibility = Visibility::Local;
  }
  else
  {
    SC_ASSERT(!isReversed || (isReversed && type.IsEdge()), ("Trying to set isReversed flag for non edge element"));
    ResolveVisibility();

    // all edges has a local visibility
    if (type.IsEdge())
      m_visibility = Visibility::Local;
  }
}

std::string const & ParsedElement::GetIdtf() const
{
  return m_idtf;
}

Visibility const ParsedElement::GetVisibility() const
{
  return m_visibility;
}

ScType const & ParsedElement::GetType() const
{
  if (m_type.IsValid())
    return m_type;

  static ScType defConst = ScType::NodeConst;
  static ScType defVar = ScType::NodeVar;
  return TypeResolver::IsConst(m_idtf) ? defConst : defVar;
}

bool ParsedElement::IsReversed() const
{
  SC_ASSERT(m_type.IsEdge(), ("You should call this method just for edges"));
  return m_isReversed;
}

std::string const & ParsedElement::GetValue() const
{
  return m_value;
}

void ParsedElement::ResolveVisibility()
{
  SC_ASSERT(m_idtf.size() > 0, ());

  // fast way
  if (m_idtf[0] == '.')
  {
    if (m_idtf.size() > 1 && m_idtf[1] == '.')
    {
      m_visibility = Visibility::Local;
    }
    else
    {
      m_visibility = Visibility::Global;
    }
  }
  else
  {
    m_visibility = Visibility::System;
  }
}

ElementHandle::ElementHandle(ElementID id)
  : m_id(id << 1)
{
}

ElementHandle::ElementHandle()
  : m_id(INVALID_ID)
{
}

ElementHandle::ElementHandle(ElementID id, bool isLocal)
{
  if (id >= INVALID_ID)
    SC_THROW_EXCEPTION(utils::ExceptionCritical, "There are too many elements in scs");

  m_id = isLocal ? 1 : 0;
  m_id |= (id << 1);
}

ElementID ElementHandle::operator * () const
{
  SC_ASSERT(IsValid(), ());
  return (m_id >> 1);
}

bool ElementHandle::IsLocal() const
{
  return (m_id & 1);
}

bool ElementHandle::IsValid() const
{
  return (m_id >> 1) != INVALID_ID;
}

bool ElementHandle::operator == (ElementHandle const & other) const
{
  return (m_id == other.m_id);
}

ElementHandle & ElementHandle::operator = (ElementHandle const & other)
{
  m_id = other.m_id;
  return *this;
}

// ---------------------------------------

Parser::Parser(class ScMemoryContext & ctx)
  : m_memoryCtx(ctx)
  , m_edgeCounter(0)
{
  m_parsedElements.reserve(PARSED_PREALLOC_NUM);
  m_parsedElementsLocal.reserve(PARSED_PREALLOC_NUM);
  m_parsedTriples.reserve(PARSED_PREALLOC_NUM);
}

bool Parser::Parse(std::string const & str)
{
  // TODO: gather result
  bool result = true;

  std::string fName;

  scsLexerTraits::InputStreamType input((ANTLR_UINT8 const *)str.c_str(),
                                        ANTLR_ENC_UTF8,
                                        (ANTLR_UINT32)str.size(),
                                        (ANTLR_UINT8*)fName.c_str());
  scsLexer lexer(&input);
  scsLexerTraits::TokenStreamType tokenStream(ANTLR_SIZE_HINT, lexer.get_tokSource());
  scsParser parser(&tokenStream);

  parser.setParser(this);
  parser.syntax();

  return result;
}

ParsedElement const & Parser::GetParsedElement(ElementHandle const & elId) const
{
  auto & container = (elId.IsLocal() ? m_parsedElementsLocal : m_parsedElements);

  if (*elId >= container.size())
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound,
    std::string("ElementId{") + std::to_string(*elId) + ", " + std::to_string(elId.IsLocal()) + "}");

  return container[*elId];
}

Parser::TripleVector const & Parser::GetParsedTriples() const
{
  return m_parsedTriples;
}

std::string Parser::GenerateEdgeIdtf()
{
  return std::string("..connector_") + std::to_string(m_edgeCounter++);
}

ElementHandle Parser::AppendElement(std::string const & idtf,
                                    ScType const & type,
                                    bool isConnectorReversed,
                                    std::string const & value)
{
  SC_CHECK_GREAT(idtf.size(), 0, ());

  ElementHandle elId;

  // try to find element
  auto const it = m_idtfToParsedElement.find(idtf);
  if (it != m_idtfToParsedElement.end())
  {
    elId = it->second;
  }
  else
  {
    // append element
    ParsedElement el(idtf, type, isConnectorReversed, value);

    bool const isLocal = (el.GetVisibility() == Visibility::Local);
    auto & container = isLocal ? m_parsedElementsLocal : m_parsedElements;

    elId = ElementHandle(static_cast<ElementID>(container.size()), isLocal);
    container.emplace_back(el);
    m_idtfToParsedElement[idtf] = elId;
  }

  return elId;
}

ElementHandle Parser::ProcessIdentifier(std::string const & name)
{
  // resolve type of sc-element
  SC_ASSERT(!IsLevel1Idtf(name), ());
  return AppendElement(name);
}

ElementHandle Parser::ProcessIdentifierLevel1(std::string const & type, std::string const & name)
{
  ScType elType = scs::TypeResolver::GetKeynodeType(type);
  if (scs::TypeResolver::IsConst(name))
  {
    elType |= ScType::Const;
  }
  else
  {
    elType |= ScType::Var;
  }

  return AppendElement(name, elType);
}

void Parser::ProcessTriple(ElementHandle const & source, ElementHandle const & edge, ElementHandle const & target)
{
  ParsedElement const & edgeEl = GetParsedElement(edge);
  SC_ASSERT(edgeEl.GetType().IsEdge(), ("edge has invalid type"));
  if (edgeEl.IsReversed())
  {
    m_parsedTriples.emplace_back(ParsedTriple(target, edge, source));
  }
  else
  {
    m_parsedTriples.emplace_back(ParsedTriple(source, edge, target));
  }    
}

ElementHandle Parser::ProcessConnector(std::string const & connector)
{ 
  ScType const type = TypeResolver::GetConnectorType(connector);
  SC_ASSERT(type.IsEdge(), ());

  return AppendElement(GenerateEdgeIdtf(), type, TypeResolver::IsConnectorReversed(connector));
}

ElementHandle Parser::ProcessContent(std::string const & content)
{
  ScType type;
  // TODO: parse content _[], [], [**], _[**]
  
  return AppendElement("...", type, false, content);
}

ElementHandle Parser::ProcessLink(std::string const & link)
{
  return AppendElement("...", ScType::Link, false, link);
}

}
