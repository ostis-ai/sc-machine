#include <gtest/gtest.h>

#include "sc_test.hpp"

extern "C"
{
#include "sc-search/search_keynodes.h"
#include "sc-utils/utils.h"
#include "sc-utils/utils_keynodes.h"
#include "sc-utils/utils_garbage_deletion.h"
}

TEST_F(ScMemoryTest, garbage_deletion_success)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  search_keynodes_initialize(context);
  sc_module_initialize(SC_TRUE, "result_structure");

  sc_event * event_garbage_deletion =
      sc_event_new(context, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_garbage_delete, 0);

  sc_addr const testAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edgeAddr = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edgeAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, setAddr, testAddr);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_erase_element, question);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_initiated, question);

  sleep(2);

  sc_iterator3 * it = sc_iterator3_f_a_f_new(context, setAddr, 0, testAddr);
  EXPECT_FALSE(sc_iterator3_next(it));
  sc_iterator3_free(it);

  sc_event_destroy(event_garbage_deletion);

  sc_memory_context_free(context);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, garbage_deletion_fail)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  search_keynodes_initialize(context);
  sc_module_initialize(SC_TRUE, "result_structure");

  sc_event * event_garbage_deletion =
      sc_event_new(context, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_garbage_delete, 0);

  sc_addr const testAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edgeAddr = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edgeAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, setAddr, testAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_init_memory_generated_structure, testAddr);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_erase_element, question);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_initiated, question);

  sleep(2);

  sc_iterator3 * it = sc_iterator3_f_a_f_new(context, setAddr, 0, testAddr);
  EXPECT_TRUE(sc_iterator3_next(it));
  sc_iterator3_free(it);

  sc_event_destroy(event_garbage_deletion);

  sc_memory_context_free(context);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, garbage_deletion_suicide)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  search_keynodes_initialize(context);
  sc_module_initialize(SC_TRUE, "result_structure");

  sc_event * event_garbage_deletion =
      sc_event_new(context, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_garbage_delete, 0);

  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edgeAddr = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edgeAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, setAddr, question);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_erase_element, question);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_initiated, question);

  sleep(2);

  sc_iterator3 * it = sc_iterator3_f_a_f_new(context, setAddr, 0, question);
  EXPECT_TRUE(sc_iterator3_next(it));
  sc_iterator3_free(it);

  sc_event_destroy(event_garbage_deletion);

  sc_memory_context_free(context);

  sc_module_shutdown();
}