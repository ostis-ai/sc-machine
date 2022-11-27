#include <gtest/gtest.h>

#include "sc_test.hpp"

extern "C"
{
#include "sc-search/search.h"
#include "sc-search/search_keynodes.h"
#include "sc-search/search_agents.h"
}

namespace SearchSetElementsTest
{

TEST_F(ScMemoryTest, agent_search_set_elements_find_4)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  sc_module_initialize();
  search_keynodes_initialize(context);

  sc_addr const set_node = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_addr const set_element_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const set_element_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const set_element_3 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const set_element_4 = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, set_node, set_element_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, set_node, set_element_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, set_node, set_element_3);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, set_node, set_element_4);


  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, set_node);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_search_all_nodes_in_set, question);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_initiated, question);

  sleep(1);

  EXPECT_TRUE(sc_helper_check_arc(context, keynode_question_finished, question, sc_type_arc_pos_const_perm));

  sc_iterator5 * it5 = sc_iterator5_f_a_a_a_f_new(
      context,
      question,
      sc_type_arc_common | sc_type_const,
      sc_type_node | sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_nrel_answer);

  EXPECT_TRUE(sc_iterator5_next(it5));

  sc_iterator3 * it3 = sc_iterator3_f_a_a_new(
      context,
      sc_iterator5_value(it5, 2),
      sc_type_arc_pos_const_perm,
      0);

  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_TRUE(sc_iterator3_next(it3));
  EXPECT_FALSE(sc_iterator3_next(it3));

  sc_iterator3_free(it3);
  sc_iterator5_free(it5);

  sc_memory_context_free(context);
  sc_module_shutdown();
}

}

