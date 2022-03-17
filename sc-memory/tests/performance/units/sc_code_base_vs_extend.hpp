/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "template_test.hpp"

class TestScCodeExtended : public TestTemplate
{
public:

  void Setup(size_t constrCount) override
  {
    ScAddr const node = m_ctx->CreateNode(ScType::NodeConstClass);
    ScAddr const attr = m_ctx->CreateNode(ScType::NodeConstRole);
    for (size_t i = 0; i < constrCount; ++i)
    {
      ScAddr const trg = m_ctx->CreateNode(ScType::NodeConstAbstract);
      ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeDCommonConst, node, trg);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, attr, edge);
    }

    m_templ.TripleWithRelation(
          node,
          ScType::EdgeDCommonVar,
          ScType::NodeVarAbstract,
          ScType::EdgeAccessVarPosPerm,
          attr);
  }
};

class TestScCodeBase : public TestTemplate
{
public:

  void Setup(size_t constrCount) override
  {
    ScAddr const kAbstract = m_ctx->CreateNode(ScType::NodeConst);
    ScAddr const kClass = m_ctx->CreateNode(ScType::NodeConst);
    ScAddr const kRole = m_ctx->CreateNode(ScType::NodeConst);
    ScAddr const kConst = m_ctx->CreateNode(ScType::NodeConst);

    ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kClass, node);
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kConst, node);

    ScAddr const attr = m_ctx->CreateNode(ScType::NodeConst);
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kRole, attr);
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kConst, attr);

    for (size_t i = 0; i < constrCount; ++i)
    {
      ScAddr const trg = m_ctx->CreateNode(ScType::NodeConstAbstract);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kAbstract, trg);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kConst, trg);

      ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeDCommonConst, node, trg);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kConst, edge);

      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, attr, edge);
    }

    m_templ.TripleWithRelation(
          node >> "_node",
          ScType::EdgeDCommonVar >> "_edge",
          ScType::NodeVarAbstract >> "_trg",
          ScType::EdgeAccessVarPosPerm,
          attr >> "_attr");
    m_templ.Triple(
          kConst,
          ScType::EdgeAccessVarPosPerm,
          "_node");
    m_templ.Triple(
          kClass,
          ScType::EdgeAccessVarPosPerm,
          "_node");
    m_templ.Triple(
          kConst,
          ScType::EdgeAccessVarPosPerm,
          "_attr");
    m_templ.Triple(
          kRole,
          ScType::EdgeAccessVarPosPerm,
          "_attr");
    m_templ.Triple(
          kAbstract,
          ScType::EdgeAccessVarPosPerm,
          "_trg");
    m_templ.Triple(
          kConst,
          ScType::EdgeAccessVarPosPerm,
          "_trg");
    m_templ.Triple(
          kConst,
          ScType::EdgeAccessVarPosPerm,
          "_edge");
  }
};
