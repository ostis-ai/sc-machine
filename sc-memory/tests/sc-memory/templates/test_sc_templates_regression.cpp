#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_scs_helper.hpp"
#include "sc-memory/sc_structure.hpp"

#include "sc_test.hpp"
#include "template_test_utils.hpp"

#include <random>

using ScTemplateRegressionTest = ScTemplateTest;

// https://github.com/ostis-dev/sc-machine/issues/224
TEST_F(ScTemplateRegressionTest, Issue224)
{
  // create template in memory
  {
    std::vector<ScAddr> contourItems;
    {
      auto testGenerateNode = [this, &contourItems](ScType const & type)
      {
        ScAddr const addr = m_ctx->GenerateNode(type);
        EXPECT_TRUE(addr.IsValid());
        contourItems.push_back(addr);
        return addr;
      };

      auto testGenerateNodeIdtf = [this, &contourItems](ScType const & type, std::string const & idtf)
      {
        ScAddr const addr = m_ctx->ResolveElementSystemIdentifier(idtf, type);
        EXPECT_TRUE(addr.IsValid());
        contourItems.push_back(addr);
        return addr;
      };

      auto testGenerateConnector = [this, &contourItems](ScType const & type, ScAddr const & src, ScAddr const & trg)
      {
        ScAddr const arcAddr = m_ctx->GenerateConnector(type, src, trg);
        EXPECT_TRUE(arcAddr.IsValid());
        contourItems.push_back(arcAddr);
        return arcAddr;
      };

      auto testGenerateConnectorAttrs = [&testGenerateConnector](
                                            ScType const & type,
                                            ScAddr const & src,
                                            ScAddr const & trg,
                                            ScType const & attrsEdgeType,
                                            std::vector<ScAddr> const & attrNodes)
      {
        ScAddr const arcAddr = testGenerateConnector(type, src, trg);
        for (ScAddr const & addr : attrNodes)
          testGenerateConnector(attrsEdgeType, addr, arcAddr);
      };

      // equal scs:
      /*
                scp_process _-> ..process_instance;;
                ..process_instance _-> rrel_1:: rrel_in:: _set1;;
                ..process_instance _-> rrel_1:: rrel_in:: _element1;;
                ..process_instance _<= nrel_decomposition_of_action:: ..proc_decomp_instance;;
                ..proc_decomp_instance _-> rrel_1:: _operator1;;
                ..proc_decomp_instance _-> _operator2;;
                ..proc_decomp_instance _-> _operator3;;
                _operator1 _<- searchElStr3;;
                _operator1 _-> rrel_1:: rrel_fixed:: rrel_scp_var:: _set1;;
                _operator1 _-> rrel_2:: rrel_assign:: rrel_arc_const_pos_perm:: rrel_scp_var:: _arc1;;
                _operator1 _-> rrel_3:: rrel_fixed:: rrel_scp_var:: _element1;;
                _operator1 _=> nrel_then:: _operator3;;
                _operator1 _=> nrel_else:: _operator2;;
                _operator2 _<- genElStr3;;
                _operator2 _-> rrel_1:: rrel_fixed:: rrel_scp_var:: _set1;;
                _operator2 _-> rrel_2:: rrel_assign:: rrel_arc_const_pos_perm:: rrel_scp_var:: _arc1;;
                _operator2 _-> rrel_3:: rrel_fixed:: rrel_scp_var:: _element1;;
                _operator2 _=> nrel_goto:: _operator3;;
                _operator3 _<- return;;
             */

      // create known nodes
      ScAddr const _set1Addr = testGenerateNodeIdtf(ScType::NodeVar, "_set1");
      ScAddr const _element1Addr = testGenerateNodeIdtf(ScType::NodeVar, "_element1");
      ScAddr const _arc1Addr = testGenerateNodeIdtf(ScType::NodeVar, "_arc1");
      ScAddr const _operator1Addr = testGenerateNodeIdtf(ScType::NodeVar, "_operator1");
      ScAddr const _operator2Addr = testGenerateNodeIdtf(ScType::NodeVar, "_operator2");
      ScAddr const _operator3Addr = testGenerateNodeIdtf(ScType::NodeVar, "_operator3");

      ScAddr const scpProcessAddr = testGenerateNodeIdtf(ScType::NodeConst, "scp_process");
      ScAddr const rrel_1Addr = testGenerateNodeIdtf(ScType::NodeConstRole, "rrel_1");
      ScAddr const rrel_2Addr = testGenerateNodeIdtf(ScType::NodeConstRole, "rrel_2");
      ScAddr const rrel_3Addr = testGenerateNodeIdtf(ScType::NodeConstRole, "rrel_3");
      ScAddr const rrel_inAddr = testGenerateNodeIdtf(ScType::NodeConstRole, "rrel_in");
      ScAddr const rrel_arc_const_pos_perm = testGenerateNodeIdtf(ScType::NodeVarRole, "rrel_arc_const_pos_perm");
      ScAddr const rrel_fixedAddr = testGenerateNodeIdtf(ScType::NodeConstRole, "rrel_fixed");
      ScAddr const rrel_assignAddr = testGenerateNodeIdtf(ScType::NodeConstRole, "rrel_assign");
      ScAddr const rrel_scp_varAddr = testGenerateNodeIdtf(ScType::NodeConstRole, "rrel_scp_var");
      ScAddr const searchElStr3Addr = testGenerateNodeIdtf(ScType::NodeConstClass, "searchElStr3");
      ScAddr const genElStr3Addr = testGenerateNodeIdtf(ScType::NodeConstClass, "geElStr3");
      ScAddr const returnAddr = testGenerateNodeIdtf(ScType::NodeConstClass, "return");
      ScAddr const nrel_decompoisition_of_actionAddr =
          testGenerateNodeIdtf(ScType::NodeVarNoRole, "nrel_decomposition_of_action");
      ScAddr const nrel_thenAddr = testGenerateNodeIdtf(ScType::NodeConstNoRole, "nrel_then");
      ScAddr const nrel_elseAddr = testGenerateNodeIdtf(ScType::NodeConstNoRole, "nrel_else");
      ScAddr const nrel_gotoAddr = testGenerateNodeIdtf(ScType::NodeConstNoRole, "nrel_goto");

      {
        // scp_process _-> ..process_instance;;
        ScAddr const __procInstanceAddr = testGenerateNode(ScType::NodeVar);
        testGenerateConnector(ScType::EdgeAccessVarPosPerm, scpProcessAddr, __procInstanceAddr);

        // ..process_instance _-> rrel_1:: rrel_in:: _set1;;
        testGenerateConnectorAttrs(
            ScType::EdgeAccessVarPosPerm,
            __procInstanceAddr,
            _set1Addr,
            ScType::EdgeAccessVarPosPerm,
            {rrel_1Addr, rrel_inAddr});

        // ..process_instance _-> rrel_1:: rrel_in::_element1;;
        testGenerateConnectorAttrs(
            ScType::EdgeAccessVarPosPerm,
            __procInstanceAddr,
            _element1Addr,
            ScType::EdgeAccessVarPosPerm,
            {rrel_1Addr, rrel_inAddr});

        // ..process_instance _<= nrel_decomposition_of_action:: ..proc_decomp_instance;;
        ScAddr const __procDecompInstanceAddr = testGenerateNode(ScType::NodeVar);
        testGenerateConnectorAttrs(
            ScType::EdgeDCommonVar,
            __procDecompInstanceAddr,
            __procInstanceAddr,
            ScType::EdgeAccessVarPosPerm,
            {nrel_decompoisition_of_actionAddr});

        // ..proc_decomp_instance _-> rrel_1:: _operator1;;
        testGenerateConnectorAttrs(
            ScType::EdgeAccessVarPosPerm,
            __procDecompInstanceAddr,
            _operator1Addr,
            ScType::EdgeAccessVarPosPerm,
            {rrel_1Addr});

        // ..proc_decomp_instance _->_operator2;;
        testGenerateConnector(ScType::EdgeAccessVarPosPerm, __procDecompInstanceAddr, _operator2Addr);

        // ..proc_decomp_instance _-> _operator3;;
        testGenerateConnector(ScType::EdgeAccessVarPosPerm, __procDecompInstanceAddr, _operator3Addr);

        // _operator1 _<- searchElStr3;;
        testGenerateConnector(ScType::EdgeAccessVarPosPerm, searchElStr3Addr, _operator1Addr);

        // _operator1 _-> rrel_1:: rrel_fixed:: rrel_scp_var:: _set1;;
        testGenerateConnectorAttrs(
            ScType::EdgeAccessVarPosPerm,
            _operator1Addr,
            _set1Addr,
            ScType::EdgeAccessVarPosPerm,
            {rrel_1Addr, rrel_fixedAddr, rrel_scp_varAddr});

        // _operator1 _-> rrel_2:: rrel_assign:: rrel_arc_const_pos_perm:: rrel_scp_var:: _arc1;;
        testGenerateConnectorAttrs(
            ScType::EdgeAccessVarPosPerm,
            _operator1Addr,
            _arc1Addr,
            ScType::EdgeAccessVarPosPerm,
            {rrel_assignAddr, rrel_arc_const_pos_perm, rrel_scp_varAddr});

        // _operator1 _-> rrel_3:: rrel_fixed:: rrel_scp_var:: _element1;;
        testGenerateConnectorAttrs(
            ScType::EdgeAccessVarPosPerm,
            _operator1Addr,
            _element1Addr,
            ScType::EdgeAccessVarPosPerm,
            {rrel_3Addr, rrel_fixedAddr, rrel_scp_varAddr});

        // _operator1 _=> nrel_then:: _operator3;;
        testGenerateConnectorAttrs(
            ScType::EdgeDCommonVar, _operator1Addr, _operator3Addr, ScType::EdgeAccessVarPosPerm, {nrel_thenAddr});

        // _operator1 _=> nrel_else:: _operator2;;
        testGenerateConnectorAttrs(
            ScType::EdgeDCommonVar, _operator1Addr, _operator2Addr, ScType::EdgeAccessVarPosPerm, {nrel_elseAddr});

        // _operator2 _<- genElStr3;;
        testGenerateConnector(ScType::EdgeAccessVarPosPerm, genElStr3Addr, _operator2Addr);

        // _operator2 _-> rrel_1:: rrel_fixed:: rrel_scp_var:: _set1;;
        testGenerateConnectorAttrs(
            ScType::EdgeAccessVarPosPerm,
            _operator2Addr,
            _set1Addr,
            ScType::EdgeAccessVarPosPerm,
            {rrel_1Addr, rrel_fixedAddr, rrel_scp_varAddr});

        // _operator2 _-> rrel_2:: rrel_assign:: rrel_arc_const_pos_perm:: rrel_scp_var:: _arc1;;
        testGenerateConnectorAttrs(
            ScType::EdgeAccessVarPosPerm,
            _operator2Addr,
            _arc1Addr,
            ScType::EdgeAccessVarPosPerm,
            {rrel_2Addr, rrel_assignAddr, rrel_arc_const_pos_perm, rrel_scp_varAddr});

        // _operator2 _-> rrel_3:: rrel_fixed:: rrel_scp_var:: _element1;;
        testGenerateConnectorAttrs(
            ScType::EdgeAccessVarPosPerm,
            _operator2Addr,
            _element1Addr,
            ScType::EdgeAccessVarPosPerm,
            {rrel_3Addr, rrel_fixedAddr, rrel_scp_varAddr});

        // _operator2 _-> rrel_3:: rrel_fixed:: rrel_scp_var:: _element1;;
        testGenerateConnectorAttrs(
            ScType::EdgeAccessVarPosPerm,
            _operator2Addr,
            _element1Addr,
            ScType::EdgeAccessVarPosPerm,
            {rrel_3Addr, rrel_fixedAddr, rrel_scp_varAddr});

        // _operator2 _=> nrel_goto:: _operator3;;
        testGenerateConnectorAttrs(
            ScType::EdgeDCommonVar, _operator2Addr, _operator3Addr, ScType::EdgeAccessVarPosPerm, {nrel_gotoAddr});

        // _operator3 _<- return;;
        testGenerateConnector(ScType::EdgeAccessVarPosPerm, returnAddr, _operator3Addr);
      }
    }

    // check helperBuildTemplate by random shuffles of contour items (to test order they returned by iterators)
    {
      auto shuffle = [&contourItems](int iterationsNum)
      {
        std::random_device rand_dev;
        std::mt19937 mt_rand(rand_dev());
        std::uniform_int_distribution<int> unif(0, static_cast<int>(contourItems.size() - 1));

        for (int i = 0; i < iterationsNum; ++i)
        {
          for (size_t j = 0; j < contourItems.size(); ++j)
          {
            size_t const newIndex = unif(mt_rand);
            std::swap(contourItems[j], contourItems[newIndex]);
          }
        }
      };

      static size_t const testNum = 100;
      for (size_t i = 0; i < testNum; ++i)
      {
        shuffle(1);
        ScAddr const structAddr =
            m_ctx->ResolveElementSystemIdentifier("test_program" + std::to_string(i), ScType::NodeConstStruct);
        EXPECT_TRUE(structAddr.IsValid());
        ScStructure contour = m_ctx->ConvertToStructure(structAddr);

        for (auto const & a : contourItems)
          contour << a;

        ScTemplate templ;
        EXPECT_TRUE(m_ctx->HelperBuildTemplate(templ, structAddr));
      }
    }
  }
}

// https://github.com/ostis-dev/sc-machine/issues/251
TEST_F(ScTemplateRegressionTest, Issue251)
{
  /**
   * k => rel: [] (* <- t;; *);;
   */

  ScAddr const kAddr = m_ctx->GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(kAddr.IsValid());
  ScAddr const relAddr = m_ctx->GenerateNode(ScType::NodeConstRole);
  EXPECT_TRUE(relAddr.IsValid());
  ScAddr const tAddr = m_ctx->GenerateNode(ScType::NodeConstClass);
  EXPECT_TRUE(tAddr.IsValid());
  ScAddr const linkAddr = m_ctx->GenerateLink();
  EXPECT_TRUE(linkAddr.IsValid());

  ScAddr const edgeK_link = m_ctx->GenerateConnector(ScType::EdgeDCommonConst, kAddr, linkAddr);
  EXPECT_TRUE(edgeK_link.IsValid());
  ScAddr const edgeT_link = m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, tAddr, linkAddr);
  EXPECT_TRUE(edgeT_link.IsValid());
  ScAddr const edgeRel_edge = m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, relAddr, edgeK_link);
  EXPECT_TRUE(edgeRel_edge.IsValid());

  // create template for a search
  ScTemplate templ;

  templ.Quintuple(kAddr, ScType::EdgeDCommonVar, ScType::Link >> "_link", ScType::EdgeAccessVarPosPerm, relAddr);
  templ.Triple(tAddr, ScType::EdgeAccessVarPosPerm, "_link");

  ScTemplateSearchResult res;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, res));

  // checks
  EXPECT_EQ(res.Size(), 1u);
  EXPECT_EQ(res[0]["_link"], linkAddr);
}

// https://github.com/ostis-dev/sc-machine/issues/295
TEST_F(ScTemplateRegressionTest, Issue295)
{
  /**
   * device_switch_multilevel _-> _x;;
   * _x _=> nrel_value::
   *     _[] (* _<= _range;; *);;
   */

  // input data
  std::string const inData =
      "device_switch_multilevel <- sc_node_class;;"
      "nrel_value <- sc_node_norole_relation;;"
      "device_switch_multilevel -> ..x;;"
      "..x => nrel_value:"
      "  [67] (* <= ..range;; *);;";

  SCsHelper scs(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(scs.GenerateBySCsText(inData));

  // find by template
  std::string const searchSCs =
      "device_switch_multilevel _-> _x;;"
      "_x _=> nrel_value::"
      "  _[] (* _<= _range;; *);;";

  ScTemplate templ;
  EXPECT_TRUE(m_ctx->HelperBuildTemplate(templ, searchSCs));

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, searchResult));

  EXPECT_EQ(searchResult.Size(), 1u);
  auto const item = searchResult[0];

  EXPECT_TRUE(item["device_switch_multilevel"].IsValid());
  EXPECT_TRUE(item["_x"].IsValid());
  EXPECT_TRUE(item["nrel_value"].IsValid());
  EXPECT_TRUE(item["_range"].IsValid());

  EXPECT_EQ(m_ctx->GetElementType(item["device_switch_multilevel"]), ScType::NodeConstClass);
  EXPECT_EQ(m_ctx->GetElementType(item["nrel_value"]), ScType::NodeConstNoRole);
}
