#include <gtest/gtest.h>

#include "sc_test.hpp"

extern "C"
{
#include "sc-search/search.h"
#include "sc-search/search_keynodes.h"
#include "sc-search/search_agents.h"
}

namespace AtomicCommandsSearchTest
{

TEST_F(ScMemoryTest, agent_search_atomic_commands_find_three_commands_in_one_decomposition)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  sc_module_initialize();
  search_keynodes_initialize(context);

  sc_addr const ui_menu_test_commands = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge1 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const,
                                          ui_menu_test_commands, keynode_ui_main_menu);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_ui_commands_decomposition, edge1);

  sc_addr const ui_menu_test_command_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const ui_menu_test_command_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const ui_menu_test_command_3 = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands, ui_menu_test_command_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands, ui_menu_test_command_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands, ui_menu_test_command_3);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_3);


  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_search_atomic_commands, question);
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
  EXPECT_FALSE(sc_iterator3_next(it3));

  sc_iterator3_free(it3);
  sc_iterator5_free(it5);

  sc_memory_context_free(context);
  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_atomic_commands_find_three_commands_in_two_decompositions)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  sc_module_initialize();
  search_keynodes_initialize(context);

  sc_addr const ui_menu_test_commands_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge1 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const,
                                          ui_menu_test_commands_1, keynode_ui_main_menu);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_ui_commands_decomposition, edge1);

  sc_addr const ui_menu_test_commands_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge2 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const,
                                          ui_menu_test_commands_2, keynode_ui_main_menu);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_ui_commands_decomposition, edge2);

  sc_addr const ui_menu_test_command_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const ui_menu_test_command_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const ui_menu_test_command_3 = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_1, ui_menu_test_command_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_1, ui_menu_test_command_2);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_2, ui_menu_test_command_3);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_3);


  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_search_atomic_commands, question);
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
  EXPECT_FALSE(sc_iterator3_next(it3));

  sc_iterator3_free(it3);
  sc_iterator5_free(it5);

  sc_memory_context_free(context);
  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_atomic_commands_find_three_commands_in_two_decompositions_but_one_command_is_in_both)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  sc_module_initialize();
  search_keynodes_initialize(context);

  sc_addr const ui_menu_test_commands_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge1 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const,
                                          ui_menu_test_commands_1, keynode_ui_main_menu);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_ui_commands_decomposition, edge1);

  sc_addr const ui_menu_test_commands_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge2 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const,
                                          ui_menu_test_commands_2, keynode_ui_main_menu);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_ui_commands_decomposition, edge2);

  sc_addr const ui_menu_test_command_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const ui_menu_test_command_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const ui_menu_test_command_3 = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_1, ui_menu_test_command_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_1, ui_menu_test_command_2);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_2, ui_menu_test_command_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_2, ui_menu_test_command_3);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_3);


  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_search_atomic_commands, question);
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
  EXPECT_FALSE(sc_iterator3_next(it3));

  sc_iterator3_free(it3);
  sc_iterator5_free(it5);

  sc_memory_context_free(context);
  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_atomic_commands_find_three_commands_in_two_decompositions_but_one_decomposition_is_in_another)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  sc_module_initialize();
  search_keynodes_initialize(context);

  sc_addr const ui_menu_test_commands_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge1 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const,
                                          ui_menu_test_commands_1, keynode_ui_main_menu);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_ui_commands_decomposition, edge1);

  sc_addr const ui_menu_test_commands_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_1, ui_menu_test_commands_2);

  sc_addr const ui_menu_test_commands_3 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge2 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const,
                                          ui_menu_test_commands_3, ui_menu_test_commands_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_ui_commands_decomposition, edge2);

  sc_addr const ui_menu_test_command_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const ui_menu_test_command_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const ui_menu_test_command_3 = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_1, ui_menu_test_command_1);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_3, ui_menu_test_command_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_3, ui_menu_test_command_3);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_3);


  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_search_atomic_commands, question);
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
  EXPECT_FALSE(sc_iterator3_next(it3));

  sc_iterator3_free(it3);
  sc_iterator5_free(it5);

  sc_memory_context_free(context);
  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_atomic_commands_find_three_commands_in_two_decompositions_but_one_decomposition_is_in_another_and_one_command_is_in_both)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  sc_module_initialize();
  search_keynodes_initialize(context);

  sc_addr const ui_menu_test_commands_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge1 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const,
                                          ui_menu_test_commands_1, keynode_ui_main_menu);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_ui_commands_decomposition, edge1);

  sc_addr const ui_menu_test_commands_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_1, ui_menu_test_commands_2);

  sc_addr const ui_menu_test_commands_3 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge2 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const,
                                          ui_menu_test_commands_3, ui_menu_test_commands_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_ui_commands_decomposition, edge2);

  sc_addr const ui_menu_test_command_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const ui_menu_test_command_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const ui_menu_test_command_3 = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_1, ui_menu_test_command_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_1, ui_menu_test_command_2);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_3, ui_menu_test_command_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_3, ui_menu_test_command_3);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_3);


  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_search_atomic_commands, question);
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
  EXPECT_FALSE(sc_iterator3_next(it3));

  sc_iterator3_free(it3);
  sc_iterator5_free(it5);

  sc_memory_context_free(context);
  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_atomic_commands_find_three_commands_in_two_decompositions_but_one_decomposition_is_in_another_and_one_command_is_in_both_and_first_composition_is_in_second)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  sc_module_initialize();
  search_keynodes_initialize(context);

  sc_addr const ui_menu_test_commands_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge1 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const,
                                          ui_menu_test_commands_1, keynode_ui_main_menu);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_ui_commands_decomposition, edge1);

  sc_addr const ui_menu_test_commands_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_1, ui_menu_test_commands_2);

  sc_addr const ui_menu_test_commands_3 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge2 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const,
                                          ui_menu_test_commands_3, ui_menu_test_commands_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_ui_commands_decomposition, edge2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_3, keynode_ui_main_menu);

  sc_addr const ui_menu_test_command_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const ui_menu_test_command_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const ui_menu_test_command_3 = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_1, ui_menu_test_command_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_1, ui_menu_test_command_2);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_3, ui_menu_test_command_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_3, ui_menu_test_command_3);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_ui_user_command_class_atom, ui_menu_test_command_3);


  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_search_atomic_commands, question);
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
  EXPECT_FALSE(sc_iterator3_next(it3));

  sc_iterator3_free(it3);
  sc_iterator5_free(it5);

  sc_memory_context_free(context);
  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_atomic_commands_find_zero_command)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  sc_module_initialize();
  search_keynodes_initialize(context);


  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_search_atomic_commands, question);
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

  EXPECT_FALSE(sc_iterator3_next(it3));

  sc_iterator3_free(it3);
  sc_iterator5_free(it5);

  sc_memory_context_free(context);
  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_atomic_commands_find_zero_command_but_there_is_a_recursion)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  sc_module_initialize();
  search_keynodes_initialize(context);

  sc_addr const ui_menu_test_commands_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edge1 = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const,
                                          ui_menu_test_commands_1, keynode_ui_main_menu);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_ui_commands_decomposition, edge1);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, ui_menu_test_commands_1, keynode_ui_main_menu);


  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_search_atomic_commands, question);
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

  EXPECT_FALSE(sc_iterator3_next(it3));

  sc_iterator3_free(it3);
  sc_iterator5_free(it5);

  sc_memory_context_free(context);
  sc_module_shutdown();
}

}
