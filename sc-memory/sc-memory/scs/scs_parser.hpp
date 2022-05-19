/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_type.hpp"

#include "sc-memory/scs/scs_types.hpp"

#include <limits>
#include <stack>

namespace scs
{

typedef uint32_t ElementID;

enum class Visibility : uint8_t
{
  Local,   // just in one file
  Global,  // in whole knowledge base, but without system idtf
  System   // system idtf
};

class ParsedElement
{
  friend class Parser;

public:
  explicit ParsedElement(
      std::string const & idtf,
      ScType const & type = ScType(),
      bool isReversed = false,
      std::string const & value = "",
      bool isURL = false);

  _SC_EXTERN std::string const & GetIdtf() const;
  _SC_EXTERN ScType const & GetType() const;

  _SC_EXTERN Visibility GetVisibility() const;

  _SC_EXTERN std::string const & GetValue() const;

  _SC_EXTERN bool IsReversed() const;
  _SC_EXTERN bool IsURL() const;

protected:
  void ResolveVisibility();

protected:
  std::string m_idtf;
  ScType m_type;
  Visibility m_visibility;  // cached value, to prevent each time check
  bool m_isReversed : 1;    // flag used just for an edges
  std::string m_value;      // string representation of content/link value
  bool m_isURL : 1;         // flag used to determine if ScLink value is an URL
};

class ElementHandle
{
public:
  _SC_EXTERN explicit ElementHandle(ElementID id);
  _SC_EXTERN ElementHandle();
  _SC_EXTERN ElementHandle(ElementID id, bool isLocal);
  _SC_EXTERN ElementHandle(ElementHandle const & other) = default;

  _SC_EXTERN ElementID operator*() const;
  _SC_EXTERN bool IsLocal() const;
  _SC_EXTERN bool IsValid() const;
  _SC_EXTERN bool operator==(ElementHandle const & other) const;
  _SC_EXTERN bool operator!=(ElementHandle const & other) const;
  _SC_EXTERN ElementHandle & operator=(ElementHandle const & other);
  _SC_EXTERN bool operator<(ElementHandle const & other) const;

private:
  static const ElementID INVALID_ID = std::numeric_limits<ElementID>::max();

  ElementID m_id;
  bool m_isLocal;
};

struct ParsedTriple
{
  ElementHandle const m_source;
  ElementHandle const m_edge;
  ElementHandle const m_target;

  ParsedTriple(ElementHandle const & s, ElementHandle const & e, ElementHandle const & t)
    : m_source(s)
    , m_edge(e)
    , m_target(t)
  {
  }
};

class Parser
{
  friend class scsParser;

  // Number of parsed elements, to preallocate container
  static const size_t PARSED_PREALLOC_NUM = 1024;

public:
  using TripleVector = std::vector<ParsedTriple>;
  using ParsedElementVector = std::vector<ParsedElement>;
  using IdtfToParsedElementMap = std::unordered_map<std::string, ElementHandle>;
  using AliasHandles = std::map<std::string, ElementHandle>;

  _SC_EXTERN Parser();

  _SC_EXTERN bool Parse(std::string const & str);
  _SC_EXTERN ParsedElement const & GetParsedElement(ElementHandle const & elID) const;
  _SC_EXTERN TripleVector const & GetParsedTriples() const;
  _SC_EXTERN std::string const & GetParseError() const;
  _SC_EXTERN AliasHandles const & GetAliases() const;

  template <typename TFunc>
  void ForEachParsedElement(TFunc && fn) const
  {
    for (auto const & el : m_parsedElementsLocal)
      fn(el);
    for (auto const & el : m_parsedElements)
      fn(el);
  }

protected:
  ParsedElement & GetParsedElementRef(ElementHandle const & elID);

  ElementHandle ResolveAlias(std::string const & name);
  ElementHandle ProcessIdentifier(std::string const & name);
  ElementHandle ProcessIdentifierLevel1(std::string const & type, std::string const & name);
  ElementHandle ProcessConnector(std::string const & connector);
  ElementHandle ProcessContent(std::string const & content, bool isVar);
  ElementHandle ProcessLink(std::string const & link);
  ElementHandle ProcessFileURL(std::string const & fileURL);

  ElementHandle ProcessEmptyContour();
  ElementHandle ProcessContourBegin();
  void ProcessContourEnd(ElementHandle const & contourHandle);
  ElementHandle ProcessContourEndWithJoin(ElementHandle const & source);

  void ProcessTriple(ElementHandle const & source, ElementHandle const & edge, ElementHandle const & target);
  void ProcessAssign(std::string const & alias, ElementHandle const & value);

private:
  ElementHandle AppendElement(
      std::string idtf,
      ScType const & type = ScType(),
      bool isConnectorReversed = false,
      std::string const & value = "",
      bool isURL = false);

  std::string GenerateNodeIdtf();
  std::string GenerateEdgeIdtf();
  std::string GenerateLinkIdtf();
  std::string GenerateContourIdtf();

private:
  ParsedElementVector m_parsedElements;
  ParsedElementVector m_parsedElementsLocal;  // just elements that has a local visibility
  std::stack<std::pair<size_t, size_t>> m_contourElementsStack;
  std::stack<size_t> m_contourTriplesStack;

  TripleVector m_parsedTriples;
  IdtfToParsedElementMap m_idtfToParsedElement;
  AliasHandles m_aliasHandles;

  std::string m_lastError;

  uint32_t m_idtfCounter;
};

}  // namespace scs
