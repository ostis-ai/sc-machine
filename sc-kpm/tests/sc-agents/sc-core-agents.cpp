#include <gtest/gtest.h>

#include "sc_test.hpp"

extern "C"
{
#include "sc-core/sc_helper.h"
#include "sc-search/search.h"
#include "sc-search/search_keynodes.h"
#include "sc-search/search_agents.h"
#include "sc-utils/utils_keynodes.h"
}

TEST_F(ScMemoryTest, agent_search_all_const_pos_outgoing_arc)
{
  sc_memory_context * context = m_ctx->GetRealContext();
  sc_addr init_memory_generated_structure;
  SC_ADDR_MAKE_EMPTY(init_memory_generated_structure);

  EXPECT_EQ(sc_module_initialize_with_init_memory_generated_structure(init_memory_generated_structure), SC_RESULT_OK);

  sc_addr const action = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, action, setAddr);

  sc_addr const addr1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, setAddr, addr1);

  sc_addr const addr2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, setAddr, addr2);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_all_output_const_pos_arc, action);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_initiated, action);

  sleep(2);
  EXPECT_TRUE(sc_helper_check_arc(context, keynode_action_finished, action, sc_type_arc_pos_const_perm));

  sc_iterator5 * it5 = sc_iterator5_f_a_a_a_f_new(
      context,
      action,
      sc_type_arc_common | sc_type_const,
      sc_type_node | sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_nrel_result);

  EXPECT_TRUE(sc_iterator5_next(it5));
  sc_addr const structure_addr = sc_iterator5_value(it5, 2);
  sc_iterator5_free(it5);

  sc_iterator3 * it3 = sc_iterator3_f_a_a_new(context, structure_addr, sc_type_arc_pos_const_perm, 0);

  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  sc_iterator3_free(it3);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_all_const_pos_outgoing_arc_with_rel)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr init_memory_generated_structure;
  SC_ADDR_MAKE_EMPTY(init_memory_generated_structure);

  EXPECT_EQ(sc_module_initialize_with_init_memory_generated_structure(init_memory_generated_structure), SC_RESULT_OK);

  sc_addr const action = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, action, setAddr);

  sc_addr const addr1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge1 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, setAddr, addr1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edge1);

  sc_addr const addr2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge2 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, setAddr, addr2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_2, edge2);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_all_output_const_pos_arc_with_rel, action);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_initiated, action);

  sleep(2);
  EXPECT_TRUE(sc_helper_check_arc(context, keynode_action_finished, action, sc_type_arc_pos_const_perm));

  sc_iterator5 * it5 = sc_iterator5_f_a_a_a_f_new(
      context,
      action,
      sc_type_arc_common | sc_type_const,
      sc_type_node | sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_nrel_result);

  EXPECT_TRUE(sc_iterator5_next(it5));
  sc_addr const structure_addr = sc_iterator5_value(it5, 2);
  sc_iterator5_free(it5);

  sc_iterator3 * it3 = sc_iterator3_f_a_a_new(context, structure_addr, sc_type_arc_pos_const_perm, 0);

  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  sc_iterator3_free(it3);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_all_const_pos_incoming_arc)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr init_memory_generated_structure;
  SC_ADDR_MAKE_EMPTY(init_memory_generated_structure);

  EXPECT_EQ(sc_module_initialize_with_init_memory_generated_structure(init_memory_generated_structure), SC_RESULT_OK);

  sc_addr const action = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, action, setAddr);

  sc_addr const addr1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, addr1, setAddr);

  sc_addr const addr2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, addr2, setAddr);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_all_input_const_pos_arc, action);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_initiated, action);

  sleep(2);
  EXPECT_TRUE(sc_helper_check_arc(context, keynode_action_finished, action, sc_type_arc_pos_const_perm));

  sc_iterator5 * it5 = sc_iterator5_f_a_a_a_f_new(
      context,
      action,
      sc_type_arc_common | sc_type_const,
      sc_type_node | sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_nrel_result);

  EXPECT_TRUE(sc_iterator5_next(it5));
  sc_addr const structure_addr = sc_iterator5_value(it5, 2);
  sc_iterator5_free(it5);

  sc_iterator3 * it3 = sc_iterator3_f_a_a_new(context, structure_addr, sc_type_arc_pos_const_perm, 0);

  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  sc_iterator3_free(it3);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_all_const_pos_incoming_arc_with_rel)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr init_memory_generated_structure;
  SC_ADDR_MAKE_EMPTY(init_memory_generated_structure);

  EXPECT_EQ(sc_module_initialize_with_init_memory_generated_structure(init_memory_generated_structure), SC_RESULT_OK);

  sc_addr const action = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, action, setAddr);

  sc_addr const addr1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge1 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, addr1, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edge1);

  sc_addr const addr2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge2 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, addr2, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_2, edge2);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_all_input_const_pos_arc_with_rel, action);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_initiated, action);

  sleep(2);
  EXPECT_TRUE(sc_helper_check_arc(context, keynode_action_finished, action, sc_type_arc_pos_const_perm));

  sc_iterator5 * it5 = sc_iterator5_f_a_a_a_f_new(
      context,
      action,
      sc_type_arc_common | sc_type_const,
      sc_type_node | sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_nrel_result);

  EXPECT_TRUE(sc_iterator5_next(it5));
  sc_addr const structure_addr = sc_iterator5_value(it5, 2);
  sc_iterator5_free(it5);

  sc_iterator3 * it3 = sc_iterator3_f_a_a_new(context, structure_addr, sc_type_arc_pos_const_perm, 0);

  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  sc_iterator3_free(it3);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_all_identifiers)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr init_memory_generated_structure;
  SC_ADDR_MAKE_EMPTY(init_memory_generated_structure);

  EXPECT_EQ(sc_module_initialize_with_init_memory_generated_structure(init_memory_generated_structure), SC_RESULT_OK);

  sc_addr const action = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, action, setAddr);

  sc_addr const noroleAddr = sc_memory_node_new(context, sc_type_node | sc_type_const | sc_type_node_norole);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_identification, noroleAddr);

  sc_addr const addr1 = sc_memory_node_new(context, sc_type_link);
  sc_addr const edge1 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const, setAddr, addr1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, noroleAddr, edge1);

  sc_addr const addr2 = sc_memory_node_new(context, sc_type_link);
  sc_addr const edge2 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const, setAddr, addr2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, noroleAddr, edge2);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_all_identifiers, action);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_initiated, action);

  sleep(2);
  EXPECT_TRUE(sc_helper_check_arc(context, keynode_action_finished, action, sc_type_arc_pos_const_perm));

  sc_iterator5 * it5 = sc_iterator5_f_a_a_a_f_new(
      context,
      action,
      sc_type_arc_common | sc_type_const,
      sc_type_node | sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_nrel_result);

  EXPECT_TRUE(sc_iterator5_next(it5));
  sc_addr const structure_addr = sc_iterator5_value(it5, 2);
  sc_iterator5_free(it5);

  sc_iterator3 * it3 = sc_iterator3_f_a_a_new(context, structure_addr, sc_type_arc_pos_const_perm, 0);

  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  sc_iterator3_free(it3);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_all_identified_elements)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr init_memory_generated_structure;
  SC_ADDR_MAKE_EMPTY(init_memory_generated_structure);

  EXPECT_EQ(sc_module_initialize_with_init_memory_generated_structure(init_memory_generated_structure), SC_RESULT_OK);

  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_link);

  sc_addr const addr1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge1 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const, addr1, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_main_idtf, edge1);

  sc_addr const addr2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge2 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const, addr2, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_main_idtf, edge2);

  sc_addr const action = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_all_identified_elements, action);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_initiated, action);

  sleep(2);
  EXPECT_TRUE(sc_helper_check_arc(context, keynode_action_finished, action, sc_type_arc_pos_const_perm));

  sc_iterator5 * it5 = sc_iterator5_f_a_a_a_f_new(
      context,
      action,
      sc_type_arc_common | sc_type_const,
      sc_type_node | sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_nrel_result);

  EXPECT_TRUE(sc_iterator5_next(it5));
  sc_addr const structure_addr = sc_iterator5_value(it5, 2);
  sc_iterator5_free(it5);

  sc_iterator3 * it3 = sc_iterator3_f_a_a_new(context, structure_addr, sc_type_arc_pos_const_perm, 0);

  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  sc_iterator3_free(it3);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_full_semantic_neighborhood)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr init_memory_generated_structure;
  SC_ADDR_MAKE_EMPTY(init_memory_generated_structure);

  EXPECT_EQ(sc_module_initialize_with_init_memory_generated_structure(init_memory_generated_structure), SC_RESULT_OK);

  sc_addr const action = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, action, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_key_sc_element, edge);

  sc_addr const idtfAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const idtfEdgeAddr = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const, idtfAddr, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_main_idtf, idtfEdgeAddr);

  sc_addr const translationAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const translationEdgeAddr =
      sc_memory_arc_new(context, sc_type_arc_common | sc_type_const, translationAddr, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_translation, translationEdgeAddr);

  sc_addr const linkAddr = sc_memory_node_new(context, sc_type_link | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, translationAddr, linkAddr);
  sc_addr const linkClass = sc_memory_node_new(context, sc_type_node | sc_type_const | sc_type_node_class);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, linkClass, linkAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_languages, linkClass);

  sc_addr const noroleAddr = sc_memory_node_new(context, sc_type_node | sc_type_const | sc_type_node_norole);

  sc_addr const decompositionAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const decompositionEdge =
      sc_memory_arc_new(context, sc_type_arc_common | sc_type_const, decompositionAddr, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, noroleAddr, decompositionEdge);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_quasybinary_relation, noroleAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nonbinary_relation, noroleAddr);

  sc_addr const decompositionLinkAddr1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, decompositionAddr, decompositionLinkAddr1);

  sc_addr const decompositionLinkAddr2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, decompositionAddr, decompositionLinkAddr2);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_full_semantic_neighborhood, action);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_initiated, action);

  sleep(2);
  EXPECT_TRUE(sc_helper_check_arc(context, keynode_action_finished, action, sc_type_arc_pos_const_perm));

  sc_iterator5 * it5 = sc_iterator5_f_a_a_a_f_new(
      context,
      action,
      sc_type_arc_common | sc_type_const,
      sc_type_node | sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_nrel_result);

  EXPECT_TRUE(sc_iterator5_next(it5));
  sc_addr const structure_addr = sc_iterator5_value(it5, 2);
  sc_iterator5_free(it5);

  sc_iterator3 * it3 = sc_iterator3_f_a_a_new(context, structure_addr, sc_type_arc_pos_const_perm, 0);

  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  sc_iterator3_free(it3);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_links_of_relation_connected_with_element)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr init_memory_generated_structure;
  SC_ADDR_MAKE_EMPTY(init_memory_generated_structure);

  EXPECT_EQ(sc_module_initialize_with_init_memory_generated_structure(init_memory_generated_structure), SC_RESULT_OK);

  sc_addr const action = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr edge = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, action, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_key_sc_element, edge);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edge);

  sc_addr const idtfAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const idtfEdgeAddr = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const, idtfAddr, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_main_idtf, idtfEdgeAddr);

  sc_addr const translationAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const translationEdgeAddr =
      sc_memory_arc_new(context, sc_type_arc_common | sc_type_const, translationAddr, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_translation, translationEdgeAddr);

  sc_addr const linkAddr = sc_memory_node_new(context, sc_type_link | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, translationAddr, linkAddr);
  sc_addr const linkClass = sc_memory_node_new(context, sc_type_node | sc_type_const | sc_type_node_class);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, linkClass, linkAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_languages, linkClass);

  sc_addr const noroleAddr = sc_memory_node_new(context, sc_type_node | sc_type_const | sc_type_node_norole);
  edge = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, action, noroleAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_2, edge);

  sc_addr const decompositionAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const decompositionEdge =
      sc_memory_arc_new(context, sc_type_arc_common | sc_type_const, decompositionAddr, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, noroleAddr, decompositionEdge);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_quasybinary_relation, noroleAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nonbinary_relation, noroleAddr);

  sc_addr const decompositionLinkAddr1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, decompositionAddr, decompositionLinkAddr1);

  sc_addr const decompositionLinkAddr2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, decompositionAddr, decompositionLinkAddr2);

  sc_memory_arc_new(
      context, sc_type_arc_pos_const_perm, keynode_action_search_links_of_relation_connected_with_element, action);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_initiated, action);

  sleep(2);
  EXPECT_TRUE(sc_helper_check_arc(context, keynode_action_finished, action, sc_type_arc_pos_const_perm));

  sc_iterator5 * it5 = sc_iterator5_f_a_a_a_f_new(
      context,
      action,
      sc_type_arc_common | sc_type_const,
      sc_type_node | sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_nrel_result);

  EXPECT_TRUE(sc_iterator5_next(it5));
  sc_addr const structure_addr = sc_iterator5_value(it5, 2);
  sc_iterator5_free(it5);

  sc_iterator3 * it3 = sc_iterator3_f_a_a_new(context, structure_addr, sc_type_arc_pos_const_perm, 0);

  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  sc_iterator3_free(it3);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_decomposition)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr init_memory_generated_structure;
  SC_ADDR_MAKE_EMPTY(init_memory_generated_structure);

  EXPECT_EQ(sc_module_initialize_with_init_memory_generated_structure(init_memory_generated_structure), SC_RESULT_OK);

  sc_addr const action = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, action, setAddr);

  sc_addr const noroleAddr = sc_memory_node_new(context, sc_type_node | sc_type_const | sc_type_node_norole);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_decomposition_relation, noroleAddr);

  sc_addr const decompositionAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge1 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const, decompositionAddr, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, noroleAddr, edge1);

  sc_addr const addr1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, decompositionAddr, addr1);

  sc_addr const addr2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, decompositionAddr, addr2);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_decomposition, action);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_action_initiated, action);

  sleep(2);
  EXPECT_TRUE(sc_helper_check_arc(context, keynode_action_finished, action, sc_type_arc_pos_const_perm));

  sc_iterator5 * it5 = sc_iterator5_f_a_a_a_f_new(
      context,
      action,
      sc_type_arc_common | sc_type_const,
      sc_type_node | sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_nrel_result);

  EXPECT_TRUE(sc_iterator5_next(it5));
  sc_addr const structure_addr = sc_iterator5_value(it5, 2);
  sc_iterator5_free(it5);

  sc_iterator3 * it3 = sc_iterator3_f_a_a_new(context, structure_addr, sc_type_arc_pos_const_perm, 0);

  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  sc_iterator3_free(it3);

  sc_module_shutdown();
}
