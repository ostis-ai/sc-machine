/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/scs/scs_parser.hpp"

#include "sc-memory/sc_debug.hpp"

#if SC_PLATFORM_WIN32
#endif

#if SC_DEBUG_MODE
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

std::string UnescapeContent(std::string const & content)
{
  std::string result = content;

  std::string::size_type pos = 0;
  while (true)
  {
    pos = result.find('\\', pos);
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
  m_isMetaElement = scs::TypeResolver::IsKeynodeType(m_idtf);

  if (!m_value.empty())
    m_visibility = Visibility::Local;
  else
  {
    ResolveVisibility();

    // all connectors has a local visibility
    if (type.IsConnector())
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

bool ParsedElement::IsMetaElement() const
{
  return m_isMetaElement;
}

ScType const & ParsedElement::GetType() const
{
  if (!m_type.IsUnknown())
    return m_type;

  static ScType defConst = ScType::ConstNode;
  static ScType defVar = ScType::VarNode;
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
  if (m_idtf.empty())
  {
    m_visibility = Visibility::System;
    return;
  }

  // fast way
  if (m_idtf[0] == '.')
  {
    if (m_idtf.size() > 1 && m_idtf[1] == '.')
      m_visibility = Visibility::Local;
    else
      m_visibility = Visibility::Global;
  }
  else
    m_visibility = Visibility::System;
}

ElementHandle::ElementHandle(ElementID id)
  : ElementHandle(id, Visibility::Global)
{
}

ElementHandle::ElementHandle()
  : ElementHandle(INVALID_ID)
{
}

ElementHandle::ElementHandle(ElementID id, Visibility visibility, bool isMetaElement)
  : m_id(id)
  , m_visibility(visibility)
  , m_isMetaElement(isMetaElement)
{
}

ElementID ElementHandle::operator*() const
{
  return m_id;
}

Visibility ElementHandle::GetVisibility() const
{
  return m_visibility;
}

bool ElementHandle::IsLocal() const
{
  return m_visibility == Visibility::Local;
}

bool ElementHandle::IsMetaElement() const
{
  return m_isMetaElement;
}

bool ElementHandle::IsValid() const
{
  return m_id != INVALID_ID;
}

bool ElementHandle::operator==(ElementHandle const & other) const
{
  return (m_id == other.m_id) && (m_visibility == other.m_visibility);
}

bool ElementHandle::operator!=(ElementHandle const & other) const
{
  return !(*this == other);
}

ElementHandle & ElementHandle::operator=(ElementHandle const & other)
{
  m_id = other.m_id;
  m_visibility = other.m_visibility;
  m_isMetaElement = other.m_isMetaElement;
  return *this;
}

bool ElementHandle::operator<(ElementHandle const & other) const
{
  if (m_id == other.m_id)
    return m_visibility < other.m_visibility;

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

ParsedElement & Parser::GetParsedElementRef(ElementHandle const & handle)
{
  ParsedElementVector & container = GetContainerByElementVisibilityRef(handle.GetVisibility());

  if (*handle >= container.size())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "ElementId{" << std::to_string(*handle) << ", " << (size_t)handle.GetVisibility() << "}");

  return container[*handle];
}

ParsedElement const & Parser::GetParsedElement(ElementHandle const & handle) const
{
  ParsedElementVector const & container = GetContainerByElementVisibility(handle.GetVisibility());

  if (*handle >= container.size())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "ElementId{" << std::to_string(*handle) << ", " << (size_t)handle.GetVisibility() << "}");

  return container[*handle];
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

std::string Parser::GenerateConnectorIdtf()
{
  return std::string("..connector_") + std::to_string(m_idtfCounter++);
}

std::string Parser::GenerateLinkIdtf()
{
  return std::string("..link_") + std::to_string(m_idtfCounter++);
}

std::string Parser::GenerateContourIdtf()
{
  return std::string("..contour_") + std::to_string(m_idtfCounter++);
}

Parser::ParsedElementVector & Parser::GetContainerByElementVisibilityRef(Visibility visibility)
{
  if (visibility == Visibility::Local)
    return m_parsedElementsLocal;

  return m_parsedElements;
}

Parser::ParsedElementVector const & Parser::GetContainerByElementVisibility(Visibility visibility) const
{
  if (visibility == Visibility::Local)
    return m_parsedElementsLocal;

  return m_parsedElements;
}

ElementHandle Parser::AppendElement(
    std::string idtf,
    ScType const & type,
    bool isConnectorReversed,
    std::string const & value /* = "" */,
    bool isURL /* = false */)
{
  if (TypeResolver::IsUnnamed(idtf))
    idtf = GenerateNodeIdtf();

  ElementHandle elementHandle;

  // try to find element
  auto const it = m_idtfToParsedElement.find(idtf);
  if (it != m_idtfToParsedElement.cend())
    elementHandle = it->second;
  else
  {
    // append element
    ParsedElement element(idtf, type, isConnectorReversed, value, isURL);
    Visibility elementVisibility = element.GetVisibility();

    ParsedElementVector & container = GetContainerByElementVisibilityRef(elementVisibility);
    elementHandle = ElementHandle(ElementID(container.size()), elementVisibility, element.IsMetaElement());
    container.emplace_back(std::move(element));

    m_idtfToParsedElement[idtf] = elementHandle;
  }

  return elementHandle;
}

ElementHandle Parser::ResolveAlias(std::string const & name)
{
  auto const it = m_aliasHandles.find(name);
  return it == m_aliasHandles.cend() ? ElementHandle() : it->second;
}

ElementHandle Parser::ProcessIdentifier(std::string const & name)
{
  // resolve type of sc-element
  ScType const type = TypeResolver::IsConst(name) ? ScType::ConstNode : ScType::VarNode;
  return AppendElement(name, type);
}

ElementHandle Parser::ProcessIdentifierLevel1(std::string const & scsType, std::string const & name)
{
  ScType type = scs::TypeResolver::GetKeynodeType(scsType);
  type |= scs::TypeResolver::IsConst(name) ? ScType::Const : ScType::Var;
  return AppendElement(name, type);
}

void Parser::ProcessTriple(
    ElementHandle const & sourceHandle,
    ElementHandle const & connectorHandle,
    ElementHandle const & targetHandle)
{
  ParsedElement & connector = GetParsedElementRef(connectorHandle);

  auto AddConnector =
      [this, &connector](
          ElementHandle const & sourceHdl, ElementHandle const & connectorHdl, ElementHandle const & targetHdl)
  {
    if (((connector.GetType() == ScType::ConstPermPosArc) || (connector.GetType() == ScType::ConstActualTempPosArc))
        && sourceHdl.IsMetaElement())
    {
      ParsedElement const & source = GetParsedElement(sourceHdl);
      ParsedElement & target = GetParsedElementRef(targetHdl);

      std::string const & sourceIdtf = source.GetIdtf();
      ScType const newType = target.m_type | scs::TypeResolver::GetKeynodeType(sourceIdtf);

      if (target.m_type.CanExtendTo(newType))
        target.m_type = newType;
      else
        SC_THROW_EXCEPTION(
            utils::ExceptionParseError,
            "Can't extend type `" << std::string(target.m_type) << "` to type `" << std::string(newType)
                                  << "` for element `" << target.GetIdtf() << "`.");

      // TODO(NikitaZotov): Unfortunately, parser collects all sc.s-elements, and only then form sc.s-triples based on
      // the parsed sc.s-elements. Due to this, it is difficult to handle cases when it is necessary not to generate a
      // sc-arc from a type of sc-elements to sc-element. Therefore, now the parser marks this arcs after that they were
      // parsed.
      connector.m_isMetaElement = true;
    }
    else
      m_parsedTriples.emplace_back(sourceHdl, connectorHdl, targetHdl);
  };

  if (connector.IsReversed())
    AddConnector(targetHandle, connectorHandle, sourceHandle);
  else
    AddConnector(sourceHandle, connectorHandle, targetHandle);
}

void Parser::ProcessAssign(std::string const & alias, ElementHandle const & value)
{
  m_aliasHandles[alias] = value;
}

ElementHandle Parser::ProcessConnector(std::string const & connector)
{
  ScType const type = TypeResolver::GetConnectorType(connector);
  return AppendElement(GenerateConnectorIdtf(), type, TypeResolver::IsConnectorReversed(connector));
}

#define DefineLinkType(content, isVar) \
  ({ \
    sc_char const startSCsLinkClassSymbol = '!'; \
    ScType _type = content.find(startSCsLinkClassSymbol) == 0 ? ScType::NodeLinkClass : ScType::NodeLink; \
    _type |= (isVar ? ScType::Var : ScType::Const); \
  })

#define ParseLinkContent(content, type) \
  if ((type & ScType::NodeLinkClass) == ScType::NodeLinkClass) \
    content = content.substr(1, content.size() - 2); \
  content = UnescapeContent(content.substr(1, content.size() - 2))

ElementHandle Parser::ProcessContent(std::string content, bool isVar)
{
  ScType const type = DefineLinkType(content, isVar);
  ParseLinkContent(content, type);
  return AppendElement(GenerateLinkIdtf(), type, false, content);
}

ElementHandle Parser::ProcessLink(ElementHandle const & handle, std::string content, bool isUrl)
{
  ParsedElement & link = GetParsedElementRef(handle);
  ScType const type = DefineLinkType(content, link.m_type.IsVar());
  ParseLinkContent(content, type);

  link.m_type = type;
  link.m_value = content;
  link.m_isURL = isUrl;

  return handle;
}

ElementHandle Parser::ProcessFileURL(std::string fileURL)
{
  ScType const type = DefineLinkType(fileURL, false);
  ParseLinkContent(fileURL, type);
  return AppendElement(GenerateLinkIdtf(), type, false, fileURL, true);
}

ElementHandle Parser::ProcessEmptyContour()
{
  return AppendElement(GenerateContourIdtf(), ScType::ConstNodeStructure);
}

void Parser::ProcessContourBegin()
{
  m_contourElementsStack.emplace(m_parsedElements.size(), m_parsedElementsLocal.size());
  m_contourTriplesStack.push(m_parsedTriples.size());
}

void Parser::ProcessContourEnd(ElementHandle const & contourHandle)
{
  size_t const last = m_parsedElements.size();
  size_t const lastLocal = m_parsedElementsLocal.size();
  size_t const lastTriple = m_parsedTriples.size();

  auto const ind = m_contourElementsStack.top();
  m_contourElementsStack.pop();

  std::set<ElementHandle> newElements;

  // append all new elements into contour
  for (size_t i = ind.first; i < last; ++i)
    newElements.insert(ElementHandle(
      ElementID(i), m_parsedElements[i].GetVisibility(), m_parsedElements[i].IsMetaElement()));

  for (size_t i = ind.second; i < lastLocal; ++i)
    newElements.insert(ElementHandle(
      ElementID(i), m_parsedElementsLocal[i].GetVisibility(), m_parsedElementsLocal[i].IsMetaElement()));

  size_t const tripleFirst = m_contourTriplesStack.top();
  m_contourTriplesStack.pop();

  for (size_t i = tripleFirst; i < lastTriple; ++i)
  {
    auto & t = m_parsedTriples[i];

    newElements.insert(t.m_source);
    newElements.insert(t.m_connector);
    newElements.insert(t.m_target);
  }

  for (auto const & elementHandle : newElements)
  {
    if (elementHandle.IsMetaElement())
      continue;

    ElementHandle const connectorHandle = ProcessConnector("->");
    ProcessTriple(contourHandle, connectorHandle, elementHandle);
  }

  ParsedElement & contour = GetParsedElementRef(contourHandle);
  contour.m_type = ScType::ConstNodeStructure;
}

}  // namespace scs
