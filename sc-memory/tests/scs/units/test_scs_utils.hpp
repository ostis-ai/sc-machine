
#pragma once

#include <gtest/gtest.h>

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_type.hpp"

#include "sc-memory/sc_debug.hpp"

#include "sc-memory/scs/scs_parser.hpp"

#define SPLIT_TRIPLE(t) \
  auto const & src = parser.GetParsedElement(t.m_source); SC_UNUSED(src); \
  auto const & edge = parser.GetParsedElement(t.m_edge); SC_UNUSED(edge); \
  auto const & trg = parser.GetParsedElement(t.m_target); SC_UNUSED(trg);

struct TripleElement
{
  TripleElement(ScType const & type)
        : m_type(type), m_visibility(scs::Visibility::System)
  {
  }

  TripleElement(ScType const & type, std::string const & idtf)
        : m_type(type), m_idtf(idtf), m_visibility(scs::Visibility::System)
  {
  }

  TripleElement(ScType const & type, std::string const & idtf, scs::Visibility const & vis)
        : m_type(type), m_idtf(idtf), m_visibility(vis)
  {
  }

  TripleElement(ScType const & type, scs::Visibility const & vis)
        : m_type(type), m_visibility(vis)
  {
  }

  void Test(scs::ParsedElement const & el) const
  {
    EXPECT_EQ(m_type, el.GetType());
    if (!m_idtf.empty())
    {
      EXPECT_EQ(m_idtf, el.GetIdtf());
    }

    EXPECT_EQ(m_visibility, el.GetVisibility());
  }

  ScType m_type;
  std::string m_idtf;
  scs::Visibility m_visibility;
};

inline std::ostream & operator<<(std::ostream & out, TripleElement const & t)
{
  SC_LOG_ERROR(
        "{ m_type: " << *t.m_type << ", m_idtf: \""
                     << t.m_idtf << "\", m_visibility: " << int(t.m_visibility) << " }");
  return out;
}


struct TripleResult
{
  void Test(scs::Parser const & parser, scs::ParsedTriple const & triple) const
  {
    auto const & src = parser.GetParsedElement(triple.m_source);
    auto const & edge = parser.GetParsedElement(triple.m_edge);
    auto const & trg = parser.GetParsedElement(triple.m_target);

    try
    {
      m_source.Test(src);
      m_edge.Test(edge);
      m_target.Test(trg);
    }
    catch (utils::ScException const & ex)
    {
      SC_LOG_ERROR(
            "\nShould be: " << std::endl
                            << " m_source: " << m_source << ", " << std::endl
                            << " m_edge: " << m_edge << ", " << std::endl
                            << " m_target: " << m_target << std::endl);

      auto const elToString = [](scs::ParsedElement const & el) -> std::string
      {
        std::stringstream ss;

        SC_LOG_ERROR("m_type: " << *el.GetType() << ", m_idtf: \"" << el.GetIdtf() << "\"");

        return ss.str();
      };

      SC_LOG_ERROR(
            "\nParsed: " << std::endl
                         << " m_source: " << elToString(src) << std::endl
                         << " m_edge: " << elToString(edge) << std::endl
                         << " m_target: " << elToString(trg) << std::endl);

      throw ex;
    }
  }

  TripleElement m_source;
  TripleElement m_edge;
  TripleElement m_target;
};


using ResultTriples = std::vector<TripleResult>;

struct TripleTester
{
  explicit TripleTester(scs::Parser const & parser) : m_parser(parser)
  {}

  void operator()(ResultTriples const & resultTriples)
  {
    auto const & triples = m_parser.GetParsedTriples();
    EXPECT_EQ(triples.size(), resultTriples.size());
    for (size_t i = 0; i < triples.size(); ++i)
      resultTriples[i].Test(m_parser, triples[i]);
  }

private:
  scs::Parser const & m_parser;
};
