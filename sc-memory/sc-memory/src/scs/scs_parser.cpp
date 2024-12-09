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
  m_isElementType = scs::TypeResolver::IsElementType(m_idtf);

  if (!m_value.empty())
    m_visibility = Visibility::Local;
  else
  {
    ResolveVisibility();

    // all connectors have a local visibility
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

bool ParsedElement::IsElementType() const
{
  return m_isElementType;
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

ElementHandle::ElementHandle(ElementID id, Visibility visibility, bool IsElementType)
  : m_id(id)
  , m_visibility(visibility)
  , m_isElementType(IsElementType)
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

bool ElementHandle::IsElementType() const
{
  return m_isElementType;
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
  m_isElementType = other.m_isElementType;
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
        "Error: Parsed element reference not found. "
        "ElementId: {"
            << std::to_string(*handle) << ", " << (size_t)handle.GetVisibility() << "}. Valid range: [0, "
            << container.size() - 1 << "].");

  return container[*handle];
}

ParsedElement const & Parser::GetParsedElement(ElementHandle const & handle) const
{
  ParsedElementVector const & container = GetContainerByElementVisibility(handle.GetVisibility());

  if (*handle >= container.size())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "Error: Parsed element not found. "
        "ElementId: {"
            << std::to_string(*handle) << ", " << (size_t)handle.GetVisibility() << "}. Valid range: [0, "
            << container.size() - 1 << "].");

  return container[*handle];
}

Parser::TripleVector const & Parser::GetParsedTriples() const
{
  return m_parsedTriples;
}

void Parser::ForEachTripleForGeneration(
    std::function<void(ParsedElement const &, ParsedElement const &, ParsedElement const &)> const & callback) const
{
  for (auto const & triple : m_parsedTriples)
  {
    auto const & source = GetParsedElement(triple.m_source);
    auto const & connector = GetParsedElement(triple.m_connector);
    auto const & target = GetParsedElement(triple.m_target);

    if (IsElementTypeOutgoingBaseArc(connector))
      continue;

    callback(source, connector, target);
  }
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
    elementHandle = ElementHandle(ElementID(container.size()), elementVisibility, element.IsElementType());
    container.emplace_back(std::move(element));

    m_idtfToParsedElement[idtf] = elementHandle;
  }

  return elementHandle;
}

bool Parser::IsElementTypeOutgoingBaseArc(ParsedElement const & element) const
{
  return m_elementTypeOutgoingBaseArcs.count(element.GetIdtf());
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
  ScType type = scs::TypeResolver::GetElementType(scsType);
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
    ParsedElement const & source = GetParsedElement(sourceHdl);
    ParsedElement & target = GetParsedElementRef(targetHdl);

    if (source.IsElementType() && (connector.GetType() == ScType::ConstPermPosArc))
    {
      std::string const & sourceIdtf = source.GetIdtf();
      ScType const sourceType = scs::TypeResolver::GetElementType(sourceIdtf);
      ScType const targetType = target.m_type;
      ScType const newTargetType = targetType | sourceType;

      if (target.IsElementType() && !newTargetType.CanExtendTo(ScType::ConstNodeClass))
        SC_THROW_EXCEPTION(
            utils::ExceptionParseError,
            "Can't extend type `" << std::string(targetType) << "` using type `" << std::string(sourceType)
                                  << "` for specified sc-element, because sc-element `" << target.GetIdtf()
                                  << "` is sc-element denoting type of sc-elements.");
      else if (ScType(targetType.BitAnd(~(ScType::Const | ScType::Var))).CanExtendTo(sourceType))
        target.m_type = newTargetType;
      else if (!sourceType.CanExtendTo(targetType))
        SC_THROW_EXCEPTION(
            utils::ExceptionParseError,
            "Can't extend type `" << std::string(targetType) << "` using type `" << std::string(sourceType)
                                  << "` for specified sc-element.");

      // TODO(NikitaZotov): Unfortunately, parser collects all sc.s-elements, and only then forms sc.s-triples based on
      // the parsed sc.s-elements. Due to this, it is difficult to handle cases when it is necessary not to generate a
      // sc-arc from a type of sc-elements to sc-element. Therefore, now the parser memorizes these arcs after they were
      // parsed.
      m_elementTypeOutgoingBaseArcs.insert(connector.GetIdtf());

      if (target.IsElementType())
        m_elementTypeNotOutgoingBaseArcsToElementTypes.insert({connector.GetIdtf(), targetHdl});
    }
    else if (source.IsElementType() || target.IsElementType())
    {
      if (source.IsElementType())
        m_elementTypeNotOutgoingBaseArcsToElementTypes.insert({connector.GetIdtf(), sourceHdl});

      if (target.IsElementType())
        m_elementTypeNotOutgoingBaseArcsToElementTypes.insert({connector.GetIdtf(), targetHdl});
    }
    else
    {
      bool const isSourceMembershipArcFromElementsType = IsElementTypeOutgoingBaseArc(source);
      bool const isTargetMembershipArcFromElementsType = IsElementTypeOutgoingBaseArc(target);

      if (isSourceMembershipArcFromElementsType || isTargetMembershipArcFromElementsType)
      {
        std::stringstream elementInfo;
        if (isSourceMembershipArcFromElementsType && target.GetType().BitAnd(ScType::Node))
          elementInfo << "with target element `" << target.GetIdtf() << "` ";
        else if (isTargetMembershipArcFromElementsType && source.GetType().BitAnd(ScType::Node))
          elementInfo << "with source element `" << source.GetIdtf() << "` ";

        SC_THROW_EXCEPTION(
            utils::ExceptionParseError,
            "Some sc.s-triple "
                << elementInfo.str()
                << "can't be generated. Outgoing constant permanent positive membership arcs denoting "
                   "the belonging of elements to sc-elements type cannot have outgoing and incoming sc-arcs.\n\n"
                   "There are several of these sc-elements types:\n"
                   "`sc_common_edge`, `sc_common_arc`, `sc_membership_arc`, `sc_main_arc`, `sc_node`, `sc_link`, "
                   "`sc_link_class`, `sc_node_tuple`, `sc_node_structure`, `sc_node_class`, `sc_node_role_relation`, "
                   "`sc_node_non_role_relation`, `sc_node_superclass`, `sc_node_material`, `sc_edge`, \n"
                   "`sc_edge_ucommon`, `sc_arc_common`, `sc_edge_dcommon`, `sc_edge_dcommon`, `sc_arc_main`, "
                   "`sc_edge_main`, `sc_arc_access`, `sc_edge_access`, `sc_node_not_binary_tuple`, `sc_node_struct`, "
                   "`sc_node_not_relation`, `sc_node_norole_relation`.\n\n"
                   "Correct usage examples:\n"
                   "\tsc_node_class -> ..set;;\n"
                   "\tsc_node_class => ..relation: ..set;;\n"
                   "\t..set => ..relation: sc_node_class;;\n"
                   "\t..node -> sc_node_class;;\n"
                   "Incorrect usage example:\n"
                   "\tsc_node_class -> rrel_1: ..set;;\n");
      }
    }

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
  ParsedElement & contour = GetParsedElementRef(contourHandle);
  if (contour.IsElementType())
    SC_THROW_EXCEPTION(
        utils::ExceptionParseError,
        "Element denoting sc-elements type `" << contour.GetIdtf() << "` can't be sc.s-contour.");

  contour.m_type = ScType::ConstNodeStructure;

  size_t const last = m_parsedElements.size();
  size_t const lastLocal = m_parsedElementsLocal.size();
  size_t const lastTriple = m_parsedTriples.size();

  auto const ind = m_contourElementsStack.top();
  m_contourElementsStack.pop();

  std::set<ElementHandle> newElements;

  // append all new elements into contour
  for (size_t i = ind.first; i < last; ++i)
    newElements.insert(
        ElementHandle(ElementID(i), m_parsedElements[i].GetVisibility(), m_parsedElements[i].IsElementType()));

  for (size_t i = ind.second; i < lastLocal; ++i)
    newElements.insert(ElementHandle(
        ElementID(i), m_parsedElementsLocal[i].GetVisibility(), m_parsedElementsLocal[i].IsElementType()));

  size_t const tripleFirst = m_contourTriplesStack.top();
  m_contourTriplesStack.pop();

  for (size_t i = tripleFirst; i < lastTriple; ++i)
  {
    auto & t = m_parsedTriples[i];

    newElements.insert(t.m_source);
    newElements.insert(t.m_connector);
    newElements.insert(t.m_target);
  }

  std::unordered_set<ElementID> addedElementTypesIntoStructure;
  for (auto const & elementHandle : newElements)
  {
    auto const & element = GetParsedElement(elementHandle);

    // Add sc-arc from structure to element type if element type has not only outgoing base sc-arcs 
    auto const & range = m_elementTypeNotOutgoingBaseArcsToElementTypes.equal_range(element.GetIdtf());
    for (auto it = range.first; it != range.second; ++it)
    {
      if (addedElementTypesIntoStructure.count(*it->second))
        continue;

      ElementHandle const connectorHandle = ProcessConnector("->");
      ProcessTriple(contourHandle, connectorHandle, it->second);
      addedElementTypesIntoStructure.insert(*it->second);
    }

    if (elementHandle.IsElementType() || IsElementTypeOutgoingBaseArc(element))
      continue;

    ElementHandle const connectorHandle = ProcessConnector("->");
    ProcessTriple(contourHandle, connectorHandle, elementHandle);
  }
}

}  // namespace scs
