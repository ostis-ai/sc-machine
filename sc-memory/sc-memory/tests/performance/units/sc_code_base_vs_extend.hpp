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
    ScAddr const node = m_ctx->GenerateNode(ScType::ConstNodeClass);
    ScAddr const attr = m_ctx->GenerateNode(ScType::ConstNodeRole);
    for (size_t i = 0; i < constrCount; ++i)
    {
      ScAddr const trg = m_ctx->GenerateNode(ScType::NodeConstAbstract);
      ScAddr const arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, node, trg);
      m_ctx->GenerateConnector(ScType::ConstPermPosArc, attr, arcAddr);
    }

    m_templ.Quintuple(
          node,
          ScType::VarCommonArc,
          ScType::NodeVarAbstract,
          ScType::VarPermPosArc,
          attr);
  }
};

class TestScCodeBase : public TestTemplate
{
public:

  void Setup(size_t constrCount) override
  {
    ScAddr const kAbstract = m_ctx->GenerateNode(ScType::ConstNode);
    ScAddr const kClass = m_ctx->GenerateNode(ScType::ConstNode);
    ScAddr const kRole = m_ctx->GenerateNode(ScType::ConstNode);
    ScAddr const kConst = m_ctx->GenerateNode(ScType::ConstNode);

    ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
    m_ctx->GenerateConnector(ScType::ConstPermPosArc, kClass, node);
    m_ctx->GenerateConnector(ScType::ConstPermPosArc, kConst, node);

    ScAddr const attr = m_ctx->GenerateNode(ScType::ConstNode);
    m_ctx->GenerateConnector(ScType::ConstPermPosArc, kRole, attr);
    m_ctx->GenerateConnector(ScType::ConstPermPosArc, kConst, attr);

    for (size_t i = 0; i < constrCount; ++i)
    {
      ScAddr const trg = m_ctx->GenerateNode(ScType::NodeConstAbstract);
      m_ctx->GenerateConnector(ScType::ConstPermPosArc, kAbstract, trg);
      m_ctx->GenerateConnector(ScType::ConstPermPosArc, kConst, trg);

      ScAddr const arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, node, trg);
      m_ctx->GenerateConnector(ScType::ConstPermPosArc, kConst, arcAddr);

      m_ctx->GenerateConnector(ScType::ConstPermPosArc, attr, arcAddr);
    }

    m_templ.Quintuple(
          node >> "_node",
          ScType::VarCommonArc >> "_arc",
          ScType::NodeVarAbstract >> "_trg",
          ScType::VarPermPosArc,
          attr >> "_attr");
    m_templ.Triple(
          kConst,
          ScType::VarPermPosArc,
          "_node");
    m_templ.Triple(
          kClass,
          ScType::VarPermPosArc,
          "_node");
    m_templ.Triple(
          kConst,
          ScType::VarPermPosArc,
          "_attr");
    m_templ.Triple(
          kRole,
          ScType::VarPermPosArc,
          "_attr");
    m_templ.Triple(
          kAbstract,
          ScType::VarPermPosArc,
          "_trg");
    m_templ.Triple(
          kConst,
          ScType::VarPermPosArc,
          "_trg");
    m_templ.Triple(
          kConst,
          ScType::VarPermPosArc,
          "_arc");
  }
};
