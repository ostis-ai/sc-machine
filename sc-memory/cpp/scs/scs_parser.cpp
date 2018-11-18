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

#include "scsLexer.h"
#include "scsParser.h"

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


class ErrorListener : public antlr4::ANTLRErrorListener
{
protected:
  void syntaxError(antlr4::Recognizer *, antlr4::Token *, size_t line,
                   size_t charPositionInLine, std::string const & msg, std::exception_ptr) override
  {
    SC_THROW_EXCEPTION(utils::ExceptionParseError, "Parse error at line " << line << "," << charPositionInLine << ": " << msg);
  }

  void reportAmbiguity(antlr4::Parser *, antlr4::dfa::DFA const &, size_t, size_t, bool,
                       antlrcpp::BitSet const &, antlr4::atn::ATNConfigSet *) override
  {
    SC_THROW_EXCEPTION(utils::ExceptionParseError, "reportAmbiguity");
  }


  void reportAttemptingFullContext(antlr4::Parser *, antlr4::dfa::DFA const &, size_t, size_t,
                                   antlrcpp::BitSet const &, antlr4::atn::ATNConfigSet *) override
  {
    SC_THROW_EXCEPTION(utils::ExceptionParseError, "reportAttemptingFullContext");
  }


  void reportContextSensitivity(antlr4::Parser *, antlr4::dfa::DFA const &, size_t, size_t,
                                size_t, antlr4::atn::ATNConfigSet *) override
  {
    SC_THROW_EXCEPTION(utils::ExceptionParseError, "reportContextSensitivity");
  }

};

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

Visibility ParsedElement::GetVisibility() const
{
  return m_visibility;
}

ScType const & ParsedElement::GetType() const
{
  if (!m_type.IsUnknown())
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
  : m_id(id)
  , m_isLocal(false)
{
}

ElementHandle::ElementHandle()
  : m_id(INVALID_ID)
  , m_isLocal(false)
{
}

ElementHandle::ElementHandle(ElementID id, bool isLocal)
  : m_id(id)
  , m_isLocal(isLocal)
{
}

ElementID ElementHandle::operator * () const
{
  SC_ASSERT(IsValid(), ());
  return m_id;
}

bool ElementHandle::IsLocal() const
{
  return m_isLocal;
}

bool ElementHandle::IsValid() const
{
  return m_id != INVALID_ID;
}

bool ElementHandle::operator == (ElementHandle const & other) const
{
  return (m_id == other.m_id) && (m_isLocal == other.m_isLocal);
}

ElementHandle & ElementHandle::operator = (ElementHandle const & other)
{
  m_id = other.m_id;
  m_isLocal = other.m_isLocal;
  return *this;
}


// ---------------------------------------

Parser::Parser()
  : m_idtfCounter(0)
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

  antlr4::ANTLRInputStream input(str);
  scsLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  scsParser parser(&tokens);

  ErrorListener errListen;

  parser.addErrorListener(&errListen);

  parser.setParser(this);
  try
  {
    parser.syntax();
  }
  catch (utils::ExceptionParseError const & e)
  {
    m_lastError = e.Message();
    result = false;
  }

  return result;
}

ParsedElement & Parser::GetParsedElementRef(ElementHandle const & elID)
{
  auto & container = (elID.IsLocal() ? m_parsedElementsLocal : m_parsedElements);

  if (*elID >= container.size())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound,
                       std::string("ElementId{") + std::to_string(*elID) + ", " + std::to_string(elID.IsLocal()) + "}");
  }

  return container[*elID];
}

ParsedElement const & Parser::GetParsedElement(ElementHandle const & elID) const
{
  auto & container = (elID.IsLocal() ? m_parsedElementsLocal : m_parsedElements);

  if (*elID >= container.size())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound,
                       std::string("ElementId{") + std::to_string(*elID) + ", " + std::to_string(elID.IsLocal()) + "}");
  }

  return container[*elID];
}

Parser::TripleVector const & Parser::GetParsedTriples() const
{
  return m_parsedTriples;
}

std::string const & Parser::GetParseError() const
{
  return m_lastError;
}

std::string Parser::GenerateEdgeIdtf()
{
  return std::string("..connector_") + std::to_string(m_idtfCounter++);
}

std::string Parser::GenerateLinkIdtf()
{
  return std::string("..link_") + std::to_string(m_idtfCounter++);
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

    elId = ElementHandle(ElementID(container.size()), isLocal);
    container.emplace_back(el);
    m_idtfToParsedElement[idtf] = elId;
  }

  return elId;
}

ElementHandle Parser::ResolveAlias(std::string const & name)
{
  auto const it = m_aliasHandles.find(name);
  return (it == m_aliasHandles.end()) ? ElementHandle() : it->second;
}

ElementHandle Parser::ProcessIdentifier(std::string const & name)
{
  // resolve type of sc-element
  SC_ASSERT(!IsLevel1Idtf(name), ());
  ScType const type = TypeResolver::IsConst(name) ? ScType::NodeConst : ScType::NodeVar;
  return AppendElement(name, type);
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

  auto addEdge = [this, &edgeEl](ElementHandle const & src, ElementHandle const & e, ElementHandle const & trg)
  {
    ParsedElement const & srcEl = GetParsedElement(src);
    std::string const & idtf = srcEl.GetIdtf();
    if (edgeEl.GetType() == ScType::EdgeAccessConstPosPerm && scs::TypeResolver::IsKeynodeType(idtf))
    {
      ParsedElement & targetEl = GetParsedElementRef(trg);
      ScType const newType = targetEl.m_type | scs::TypeResolver::GetKeynodeType(idtf);

      if (targetEl.m_type.CanExtendTo(newType))
      {
        targetEl.m_type = newType;
      }
      else
      {
        SC_THROW_EXCEPTION(utils::ExceptionParseError,
                           "Can't merge types for element " + targetEl.GetIdtf());
      }
    }
    else
    {
      m_parsedTriples.emplace_back(ParsedTriple(src, e, trg));
    }
  };

  if (edgeEl.IsReversed())
  {
    addEdge(target, edge, source);
  }
  else
  {
    addEdge(source, edge, target);
  }
}

void Parser::ProcessAssign(std::string const & alias, ElementHandle const & value)
{
  m_aliasHandles[alias] = value;
}

ElementHandle Parser::ProcessConnector(std::string const & connector)
{
  ScType const type = TypeResolver::GetConnectorType(connector);
  SC_ASSERT(type.IsEdge(), ());

  return AppendElement(GenerateEdgeIdtf(), type, TypeResolver::IsConnectorReversed(connector));
}

ElementHandle Parser::ProcessContent(std::string const & content)
{
  SC_CHECK(content.size() > 0, ());

  ScType type = utils::StringUtils::StartsWith(content, "_", true) ? ScType::Var : ScType::Const;
  if (utils::StringUtils::StartsWith(content, "[*") || utils::StringUtils::StartsWith(content, "_[*"))
  {
    type |= ScType::NodeStruct;
  }
  else if (utils::StringUtils::StartsWith(content, "[") || utils::StringUtils::StartsWith(content, "_["))
  {
    type |= ScType::Link;
  }

  return AppendElement(GenerateLinkIdtf(), type, false, content);
}

ElementHandle Parser::ProcessLink(std::string const & link)
{
  return AppendElement(GenerateLinkIdtf(), ScType::Link, false, link);
}

}
