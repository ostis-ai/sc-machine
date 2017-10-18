/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "../sc_types.hpp"
#include "scs_parseutils.hpp"
#include "scs_types.hpp"

#include <stack>

namespace scs
{

typedef uint32_t ElementID;

enum class Visibility : uint8_t
{
  Local,          // just in one file
  Global,         // in whole knowledge base, but without system idtf
  System          // system idtf
};

class ParsedElement
{
  friend class Parser;

public:
  explicit ParsedElement(std::string const & idtf,
                         ScType const & type = ScType(),
                         bool isReversed = false,
                         std::string const & value = "");

  _SC_EXTERN std::string const & GetIdtf() const;
  _SC_EXTERN Visibility const GetVisibility() const;
  _SC_EXTERN ScType const & GetType() const;
  _SC_EXTERN bool IsReversed() const;
  _SC_EXTERN std::string const & GetValue() const;

protected:
  void ResolveVisibility();

protected:
  std::string m_idtf;
  ScType m_type;
  Visibility m_visibility; // cached value, to prevent each time check
  bool m_isReversed : 1;   // flag used just for an edges
  std::string m_value;  // string representation of content/link value 
};

class ElementHandle
{
public:
  explicit ElementHandle(ElementID id);
  ElementHandle(); 
  ElementHandle(ElementID id, bool isLocal);

  ElementID operator * () const;
  _SC_EXTERN bool IsLocal() const;
  _SC_EXTERN bool IsValid() const;
  _SC_EXTERN bool operator == (ElementHandle const & other) const;
  ElementHandle & operator = (ElementHandle const & other);

private:
  static const ElementID INVALID_ID = ((1 << 30) - 1);

  ElementID m_id : 32;
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
  using IdtfToParsedElementMap = std::map<std::string, ElementHandle>;

  _SC_EXTERN explicit Parser(class ScMemoryContext & ctx);

  _SC_EXTERN bool Parse(std::string const & str);
  _SC_EXTERN ParsedElement const & GetParsedElement(ElementHandle const & elID) const;
  _SC_EXTERN TripleVector const & GetParsedTriples() const;
  _SC_EXTERN std::string const & GetParseError() const;

protected:

  ParsedElement & GetParsedElementRef(ElementHandle const & elID);

  ElementHandle ProcessIdentifier(std::string const & name);
  ElementHandle ProcessIdentifierLevel1(std::string const & type, std::string const & name);
  void ProcessTriple(ElementHandle const & source, ElementHandle const & edge, ElementHandle const & target);
  ElementHandle ProcessConnector(std::string const & connector);
  ElementHandle ProcessContent(std::string const & content);
  ElementHandle ProcessLink(std::string const & link);

private:
  ElementHandle AppendElement(std::string const & idtf,
                              ScType const & type = ScType(),
                              bool isConnectorReversed = false,
                              std::string const & value = "");

  std::string GenerateEdgeIdtf();
   
private:
  class ScMemoryContext & m_memoryCtx;

  ParsedElementVector m_parsedElements;
  ParsedElementVector m_parsedElementsLocal;   // just elements that has a local visibility
  TripleVector m_parsedTriples;
  IdtfToParsedElementMap m_idtfToParsedElement;

  std::string m_lastError;

  uint32_t m_edgeCounter;
};

}
