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

sc_addr test_generate_elements_and_call_agent_search_all_const_pos_incoming_arc(
    sc_memory_context * context,
    sc_addr const addr)
{
  sc_addr const setAddr1 = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_uint32 const MULTIPLE_ARCS_COUNT = 10;
  for (sc_uint32 i = 0; i < MULTIPLE_ARCS_COUNT; ++i)
  {
    sc_memory_arc_new(context, sc_type_const_perm_pos_arc, setAddr1, addr);
  }

  sc_uint32 const MULTIPLE_NODES_COUNT = 10;
  for (sc_uint32 i = 0; i < MULTIPLE_NODES_COUNT; ++i)
  {
    sc_addr const setAddr2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
    sc_memory_arc_new(context, sc_type_const_perm_pos_arc, setAddr2, addr);
  }

  sc_addr const action_addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, action_addr, addr);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_all_input_const_pos_arc, action_addr);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_initiated, action_addr);

  return action_addr;
}

sc_addr test_generate_elements_and_call_agent_search_all_const_pos_outgoing_arc(
    sc_memory_context * context,
    sc_addr const addr)
{
  sc_addr const setAddr1 = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_uint32 const MULTIPLE_ARCS_COUNT = 10;
  for (sc_uint32 i = 0; i < MULTIPLE_ARCS_COUNT; ++i)
  {
    sc_memory_arc_new(context, sc_type_const_perm_pos_arc, addr, setAddr1);
  }

  sc_uint32 const MULTIPLE_NODES_COUNT = 10;
  for (sc_uint32 i = 0; i < MULTIPLE_NODES_COUNT; ++i)
  {
    sc_addr const setAddr2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
    sc_memory_arc_new(context, sc_type_const_perm_pos_arc, addr, setAddr2);
  }

  sc_addr const action_addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, action_addr, addr);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_all_output_const_pos_arc, action_addr);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_initiated, action_addr);

  return action_addr;
}

sc_addr test_generate_elements_and_call_agent_search_full_semantic_neighborhood(
    sc_memory_context * context,
    sc_addr const addr)
{
  sc_addr const idtf_addr = sc_memory_node_new(context, sc_type_node_link | sc_type_const);  // idtf_addr = {}
  sc_addr const idtf_edge_addr =
      sc_memory_arc_new(context, sc_type_common_arc | sc_type_const, idtf_addr, addr);  // addr => idtf_addr
  sc_memory_arc_new(
      context,
      sc_type_const_perm_pos_arc,
      keynode_nrel_main_idtf,
      idtf_edge_addr);  // addr => keynode_nrel_main_idtf: idtf_addr

  sc_addr const translation_addr = sc_memory_node_new(context, sc_type_node | sc_type_const);  // translation
  sc_addr const translation_edge_addr =
      sc_memory_arc_new(context, sc_type_common_arc | sc_type_const, translation_addr, addr);  // translation => addr
  sc_memory_arc_new(
      context,
      sc_type_const_perm_pos_arc,
      keynode_nrel_translation,
      translation_edge_addr);  // translation => keynode_nrel_translation: addr

  sc_addr const link_addr = sc_memory_node_new(context, sc_type_node_link | sc_type_const);  // link
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, translation_addr, link_addr);       // translation -> link
  sc_addr const link_class_addr =
      sc_memory_node_new(context, sc_type_node | sc_type_const | sc_type_node_class);  // class
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, link_class_addr, link_addr);  // class -> link
  sc_memory_arc_new(
      context, sc_type_const_perm_pos_arc, keynode_languages, link_class_addr);  // keynode_languages -> class

  sc_addr const norole_addr =
      sc_memory_node_new(context, sc_type_node | sc_type_const | sc_type_node_norole);  // norole

  sc_addr const decomposition_addr = sc_memory_node_new(context, sc_type_node | sc_type_const);  // decomposition
  sc_addr const decomposition_edge_addr = sc_memory_arc_new(
      context, sc_type_common_arc | sc_type_const, decomposition_addr, addr);  // decomposition => addr
  sc_memory_arc_new(
      context, sc_type_const_perm_pos_arc, norole_addr, decomposition_edge_addr);  // decomposition => norole: addr
  sc_memory_arc_new(
      context, sc_type_const_perm_pos_arc, keynode_quasybinary_relation, norole_addr);  //?? quasybinary -> norole
  sc_memory_arc_new(
      context, sc_type_const_perm_pos_arc, keynode_nonbinary_relation, norole_addr);  //??? nonbinary -> norole

  sc_addr const decomposition_link_addr1 =
      sc_memory_node_new(context, sc_type_node | sc_type_const);  // decomposition_link1
  sc_memory_arc_new(
      context,
      sc_type_const_perm_pos_arc,
      decomposition_addr,
      decomposition_link_addr1);  // decomposition -> decomposition_link1

  sc_addr const decomposition_link_addr2 =
      sc_memory_node_new(context, sc_type_node | sc_type_const);  // decomposition_link2
  sc_memory_arc_new(
      context,
      sc_type_const_perm_pos_arc,
      decomposition_addr,
      decomposition_link_addr2);  // decomposition -> decomposition_link2

  sc_addr const action_addr = sc_memory_node_new(context, sc_type_node | sc_type_const);  // action_addr
  sc_addr const edge_addr =
      sc_memory_arc_new(context, sc_type_const_perm_pos_arc, action_addr, addr);  // action_addr -> addr
  sc_memory_arc_new(
      context,
      sc_type_const_perm_pos_arc,
      keynode_rrel_key_sc_element,
      edge_addr);  // action_addr -> keynode_rrel_key_sc_element: addr
  sc_memory_arc_new(
      context,
      sc_type_const_perm_pos_arc,
      keynode_action_full_semantic_neighborhood,
      action_addr);  // keynode_action_full_semantic_neighborhood -> action_addr
  sc_memory_arc_new(
      context,
      sc_type_const_perm_pos_arc,
      keynode_action_initiated,
      action_addr);  // keynode_action_initiated -> action_addr

  return action_addr;
}

sc_bool test_agent_finished(sc_memory_context * context, sc_addr const action_addr)
{
  return sc_helper_check_arc(context, keynode_action_finished, action_addr, sc_type_const_perm_pos_arc);
}

sc_bool test_agent_has_result(sc_memory_context * context, sc_addr const action_addr)
{
  sc_iterator5 * action_addr_it5 = sc_iterator5_f_a_a_a_f_new(
      context,
      action_addr,
      sc_type_common_arc | sc_type_const,
      sc_type_node | sc_type_const,  // action_addr -> keynode_nrel_result {}
      sc_type_const_perm_pos_arc,
      keynode_nrel_result);
  sc_bool const result = sc_iterator5_next(action_addr_it5);
  sc_iterator5_free(action_addr_it5);

  return result;
}

TEST_F(ScMemoryTest, agents_parallel_work1)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr init_memory_generated_structure;
  SC_ADDR_MAKE_EMPTY(init_memory_generated_structure);

  sc_module_initialize_with_init_memory_generated_structure(init_memory_generated_structure);

  sc_addr const addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const action_addr1 = test_generate_elements_and_call_agent_search_all_const_pos_incoming_arc(context, addr);
  sc_addr const action_addr2 = test_generate_elements_and_call_agent_search_full_semantic_neighborhood(context, addr);
  sc_addr const action_addr3 = test_generate_elements_and_call_agent_search_all_const_pos_outgoing_arc(context, addr);
  sc_addr const action_addr4 = test_generate_elements_and_call_agent_search_full_semantic_neighborhood(context, addr);

  sleep(2);

  EXPECT_TRUE(test_agent_finished(context, action_addr1));
  EXPECT_TRUE(test_agent_has_result(context, action_addr1));
  EXPECT_TRUE(test_agent_finished(context, action_addr2));
  EXPECT_TRUE(test_agent_has_result(context, action_addr2));
  EXPECT_TRUE(test_agent_finished(context, action_addr3));
  EXPECT_TRUE(test_agent_has_result(context, action_addr3));
  EXPECT_TRUE(test_agent_finished(context, action_addr4));
  EXPECT_TRUE(test_agent_has_result(context, action_addr4));

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agents_parallel_work2)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr init_memory_generated_structure;
  SC_ADDR_MAKE_EMPTY(init_memory_generated_structure);

  sc_module_initialize_with_init_memory_generated_structure(init_memory_generated_structure);

  sc_addr const addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const action_addr1 = test_generate_elements_and_call_agent_search_full_semantic_neighborhood(context, addr);
  sc_addr const action_addr2 = test_generate_elements_and_call_agent_search_all_const_pos_incoming_arc(context, addr);
  sc_addr const action_addr3 = test_generate_elements_and_call_agent_search_all_const_pos_outgoing_arc(context, addr);
  sc_addr const action_addr4 = test_generate_elements_and_call_agent_search_full_semantic_neighborhood(context, addr);
  sc_addr const action_addr5 = test_generate_elements_and_call_agent_search_all_const_pos_outgoing_arc(context, addr);
  sc_addr const action_addr6 = test_generate_elements_and_call_agent_search_all_const_pos_incoming_arc(context, addr);
  sc_addr const action_addr7 = test_generate_elements_and_call_agent_search_full_semantic_neighborhood(context, addr);

  sleep(2);

  EXPECT_TRUE(test_agent_finished(context, action_addr1));
  EXPECT_TRUE(test_agent_has_result(context, action_addr1));
  EXPECT_TRUE(test_agent_finished(context, action_addr2));
  EXPECT_TRUE(test_agent_has_result(context, action_addr2));
  EXPECT_TRUE(test_agent_finished(context, action_addr3));
  EXPECT_TRUE(test_agent_has_result(context, action_addr3));
  EXPECT_TRUE(test_agent_finished(context, action_addr4));
  EXPECT_TRUE(test_agent_has_result(context, action_addr4));
  EXPECT_TRUE(test_agent_finished(context, action_addr5));
  EXPECT_TRUE(test_agent_has_result(context, action_addr5));
  EXPECT_TRUE(test_agent_finished(context, action_addr6));
  EXPECT_TRUE(test_agent_has_result(context, action_addr6));
  EXPECT_TRUE(test_agent_finished(context, action_addr7));
  EXPECT_TRUE(test_agent_has_result(context, action_addr7));

  sc_module_shutdown();
}
