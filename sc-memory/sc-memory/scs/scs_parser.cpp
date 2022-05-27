/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/scs/scs_parser.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_debug.hpp"

#include <limits>

#if SC_PLATFORM_WIN32
#endif

#if SC_DEBUG_MODE
/// TODO: @deniskoronchik: make as cmake parameter
#  define SCS_DEBUG_TOKEN 1
#endif

#if SCS_DEBUG_TOKEN
#  define DEBUG_TOKEN(__msg) \
    { \
      std::cout << __msg << std::endl; \
    }
#else
#  define DEBUG_TOKEN(__msg)
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

std::string UnescapeContent(std::string const & content)
{
  std::string result = content;

  std::string::size_type pos = 0;
  while (true)
  {
    pos = result.find("\\", pos);
    if (pos == std::string::npos)
      break;

    if (pos < result.size() - 1)
    {
      auto const nextSymbol = result[pos + 1];
      if (nextSymbol == '\\' || nextSymbol == '[' || nextSymbol == ']')
        result.replace(pos, 2, {nextSymbol});
    }

    ++pos;
  }

  return result;
}

class ErrorListener : public antlr4::ANTLRErrorListener
{
protected:
  void syntaxError(
      antlr4::Recognizer *,
      antlr4::Token *,
      size_t line,
      size_t charPositionInLine,
      std::string const & msg,
      std::exception_ptr) override
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionParseError, "Parse error at line " << line << "," << charPositionInLine << ": " << msg);
  }

  void reportAmbiguity(
      antlr4::Parser *,
      antlr4::dfa::DFA const &,
      size_t,
      size_t,
      bool,
      antlrcpp::BitSet const &,
      antlr4::atn::ATNConfigSet *) override
  {
    SC_THROW_EXCEPTION(utils::ExceptionParseError, "reportAmbiguity");
  }

  void reportAttemptingFullContext(
      antlr4::Parser *,
      antlr4::dfa::DFA const &,
      size_t,
      size_t,
      antlrcpp::BitSet const &,
      antlr4::atn::ATNConfigSet *) override
  {
    SC_THROW_EXCEPTION(utils::ExceptionParseError, "reportAttemptingFullContext");
  }

  void reportContextSensitivity(
      antlr4::Parser *,
      antlr4::dfa::DFA const &,
      size_t,
      size_t,
      size_t,
      antlr4::atn::ATNConfigSet *) override
  {
    SC_THROW_EXCEPTION(utils::ExceptionParseError, "reportContextSensitivity");
  }
};

}  // namespace

namespace scs
{

ParsedElement::ParsedElement(
    std::string const & idtf,
    ScType const & type,
    bool isReversed,
    std::string const & value /* = "" */,
    bool isURL /* = false */)
  : m_idtf(idtf)
  , m_type(type)
  , m_visibility(Visibility::System)
  , m_isReversed(isReversed)
  , m_value(value)
  , m_isURL(isURL)
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
  return m_isReversed;
}

std::string const & ParsedElement::GetValue() const
{
  return m_value;
}

bool ParsedElement::IsURL() const
{
  return m_isURL;
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

ElementID ElementHandle::operator*() const
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

bool ElementHandle::operator==(ElementHandle const & other) const
{
  return (m_id == other.m_id) && (m_isLocal == other.m_isLocal);
}

bool ElementHandle::operator!=(ElementHandle const & other) const
{
  return !(*this == other);
}

ElementHandle & ElementHandle::operator=(ElementHandle const & other)
{
  m_id = other.m_id;
  m_isLocal = other.m_isLocal;
  return *this;
}

bool ElementHandle::operator<(ElementHandle const & other) const
{
  if (m_id == other.m_id)
    return m_isLocal < other.m_isLocal;

  return m_id < other.m_id;
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

  ErrorListener errListener;

  parser.addErrorListener(&errListener);
  lexer.addErrorListener(&errListener);

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

  parser.removeErrorListener(&errListener);
  lexer.removeErrorListener(&errListener);

  return result;
}

ParsedElement & Parser::GetParsedElementRef(ElementHandle const & elID)
{
  auto & container = (elID.IsLocal() ? m_parsedElementsLocal : m_parsedElements);

  if (*elID >= container.size())
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        std::string("ElementId{") + std::to_string(*elID) + ", " + std::to_string(elID.IsLocal()) + "}");
  }

  return container[*elID];
}

ParsedElement const & Parser::GetParsedElement(ElementHandle const & elID) const
{
  auto & container = (elID.IsLocal() ? m_parsedElementsLocal : m_parsedElements);

  if (*elID >= container.size())
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
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

Parser::AliasHandles const & Parser::GetAliases() const
{
  return m_aliasHandles;
}

std::string Parser::GenerateNodeIdtf()
{
  return std::string("..node_") + std::to_string(m_idtfCounter++);
}

std::string Parser::GenerateEdgeIdtf()
{
  return std::string("..edge_") + std::to_string(m_idtfCounter++);
}

std::string Parser::GenerateLinkIdtf()
{
  return std::string("..link_") + std::to_string(m_idtfCounter++);
}

std::string Parser::GenerateContourIdtf()
{
  return std::string("..contour_") + std::to_string(m_idtfCounter++);
}

ElementHandle Parser::AppendElement(
    std::string idtf,
    ScType const & type,
    bool isConnectorReversed,
    std::string const & value /* = "" */,
    bool isURL /* = false */)
{
  SC_CHECK_GREAT(idtf.size(), 0, ());
  if (TypeResolver::IsUnnamed(idtf))
    idtf = GenerateNodeIdtf();

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
    ParsedElement el(idtf, type, isConnectorReversed, value, isURL);

    bool const isLocal = (el.GetVisibility() == Visibility::Local);
    auto & container = isLocal ? m_parsedElementsLocal : m_parsedElements;

    elId = ElementHandle(ElementID(container.size()), isLocal);
    container.emplace_back(std::move(el));
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

  auto addEdge = [this, &edgeEl](ElementHandle const & src, ElementHandle const & e, ElementHandle const & trg) {
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
        SC_THROW_EXCEPTION(utils::ExceptionParseError, "Can't merge types for element " + targetEl.GetIdtf());
      }

      if (!m_contourTriplesStack.empty())
        m_parsedTriples.emplace_back(src, e, trg);
    }
    else
    {
      m_parsedTriples.emplace_back(src, e, trg);
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

ElementHandle Parser::ProcessContent(std::string const & content, bool isVar)
{
  ScType type = ScType::Link;

  type |= (isVar ? ScType::Var : ScType::Const);

  return AppendElement(GenerateLinkIdtf(), type, false, UnescapeContent(content));
}

ElementHandle Parser::ProcessLink(std::string const & link)
{
  return AppendElement(GenerateLinkIdtf(), ScType::Link, false, link);
}

ElementHandle Parser::ProcessFileURL(std::string const & fileURL)
{
  return AppendElement(GenerateLinkIdtf(), ScType::LinkConst, false, fileURL, true);
}

ElementHandle Parser::ProcessEmptyContour()
{
  return AppendElement(GenerateContourIdtf(), ScType::NodeConstStruct);
}

ElementHandle Parser::ProcessContourBegin()
{
  ElementHandle const result = AppendElement(GenerateContourIdtf(), ScType::NodeConstStruct);

  m_contourElementsStack.push(std::make_pair(m_parsedElements.size(), m_parsedElementsLocal.size()));
  m_contourTriplesStack.push(m_parsedTriples.size());

  return result;
}

void Parser::ProcessContourEnd(ElementHandle const & contourHandle)
{
  SC_ASSERT(!m_contourElementsStack.empty(), ());
  SC_ASSERT(!m_contourTriplesStack.empty(), ());

  size_t const last = m_parsedElements.size();
  size_t const lastLocal = m_parsedElementsLocal.size();
  size_t const lastTriple = m_parsedTriples.size();

  auto const ind = m_contourElementsStack.top();
  m_contourElementsStack.pop();

  std::set<ElementHandle> newElements;

  // append all new elements into contour
  for (size_t i = ind.first; i < last; ++i)
    newElements.insert(ElementHandle(ElementID(i), false));

  for (size_t i = ind.second; i < lastLocal; ++i)
    newElements.insert(ElementHandle(ElementID(i), true));

  size_t const tripleFirst = m_contourTriplesStack.top();
  m_contourTriplesStack.pop();

  for (size_t i = tripleFirst; i < lastTriple; ++i)
  {
    auto & t = m_parsedTriples[i];

    newElements.insert(t.m_source);
    newElements.insert(t.m_edge);
    newElements.insert(t.m_target);
  }

  for (auto const & el : newElements)
  {
    ElementHandle const edge = ProcessConnector("->");
    ProcessTriple(contourHandle, edge, el);
  }
}

ElementHandle Parser::ProcessContourEndWithJoin(ElementHandle const & source)
{
  SC_ASSERT(!m_contourElementsStack.empty(), ());
  SC_ASSERT(!m_contourTriplesStack.empty(), ());

  size_t const last = m_parsedElements.size();
  size_t const lastTriple = m_parsedTriples.size();

  auto const ind = m_contourElementsStack.top();
  m_contourElementsStack.pop();

  std::set<ElementHandle> newElements;

  // append all new elements into contour
  for (size_t i = ind.first; i < last; ++i)
    newElements.insert(ElementHandle(ElementID(i), false));

  size_t const tripleFirst = m_contourTriplesStack.top();
  m_contourTriplesStack.pop();

  for (size_t i = tripleFirst; i < lastTriple; ++i)
  {
    auto const & t = m_parsedTriples[i];
    newElements.insert(t.m_source);
    newElements.insert(t.m_edge);
    newElements.insert(t.m_target);
  }

  for (auto const & el : newElements)
  {
    ElementHandle const edge = ProcessConnector("->");
    ProcessTriple(source, edge, el);
  }

  ParsedElement & srcEl = GetParsedElementRef(source);
  srcEl.m_type = ScType::NodeConstStruct;
  return source;
}

}  // namespace scs
