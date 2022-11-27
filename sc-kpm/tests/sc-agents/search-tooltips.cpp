#include <gtest/gtest.h>

#include "sc_test.hpp"

extern "C"
{
#include "sc-search/search.h"
#include "sc-search/search_keynodes.h"
#include "sc-search/search_agents.h"
}

namespace SearchTooltipsTest
{


TEST_F(ScMemoryTest, agent_search_tooltip_find_definition)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  sc_module_initialize();
  search_keynodes_initialize(context);

  sc_addr const target = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const key_sc_element = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const sc_text_translation = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const text_link = sc_memory_node_new(context, sc_type_node | sc_type_const | sc_type_link);
  sc_addr const lang_ru = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_addr edge1 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, key_sc_element, target);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_key_sc_element, edge1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_sc_definition, key_sc_element);
  sc_addr edge2 = sc_memory_arc_new(context,
                                    sc_type_arc_common | sc_type_const, sc_text_translation, key_sc_element);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_sc_text_translation_value, edge2);


  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sc_text_translation, text_link);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, lang_ru, text_link);


  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr edge3 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, target);
  sc_addr edge4 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, lang_ru);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edge3);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_2, edge4);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_search_tooltip, question);
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
  EXPECT_FALSE(sc_iterator3_next(it3));

  sc_iterator3_free(it3);
  sc_iterator5_free(it5);

  sc_memory_context_free(context);
  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_tooltip_find_explanation)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  sc_module_initialize();
  search_keynodes_initialize(context);

  sc_addr const target = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const key_sc_element = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const sc_text_translation = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const text_link = sc_memory_node_new(context, sc_type_node | sc_type_const | sc_type_link);
  sc_addr const lang_ru = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_addr edge1 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, key_sc_element, target);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_key_sc_element, edge1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_sc_explanation, key_sc_element);
  sc_addr edge2 = sc_memory_arc_new(context,
                                    sc_type_arc_common | sc_type_const, sc_text_translation, key_sc_element);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_sc_text_translation_value, edge2);


  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sc_text_translation, text_link);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, lang_ru, text_link);


  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr edge3 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, target);
  sc_addr edge4 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, lang_ru);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edge3);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_2, edge4);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_search_tooltip, question);
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
  EXPECT_FALSE(sc_iterator3_next(it3));

  sc_iterator3_free(it3);
  sc_iterator5_free(it5);

  sc_memory_context_free(context);
  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_tooltip_find_note)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  sc_module_initialize();
  search_keynodes_initialize(context);

  sc_addr const target = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const key_sc_element = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const sc_text_translation = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const text_link = sc_memory_node_new(context, sc_type_node | sc_type_const | sc_type_link);
  sc_addr const lang_ru = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_addr edge1 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, key_sc_element, target);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_key_sc_element, edge1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_sc_note, key_sc_element);
  sc_addr edge2 = sc_memory_arc_new(context,
                                    sc_type_arc_common | sc_type_const, sc_text_translation, key_sc_element);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_sc_text_translation_value, edge2);


  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sc_text_translation, text_link);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, lang_ru, text_link);


  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr edge3 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, target);
  sc_addr edge4 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, lang_ru);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edge3);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_2, edge4);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_search_tooltip, question);
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
  EXPECT_FALSE(sc_iterator3_next(it3));

  sc_iterator3_free(it3);
  sc_iterator5_free(it5);

  sc_memory_context_free(context);
  sc_module_shutdown();
}

TEST_F(ScMemoryTest, agent_search_tooltip_find_definition_because_it_has_higher_priority_then_explanation)
{
  sc_memory_context * context = sc_memory_context_new(sc_access_lvl_make_min);

  sc_module_initialize();
  search_keynodes_initialize(context);

  sc_addr const target = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const key_sc_element_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const sc_text_translation_1 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const text_link_1 = sc_memory_node_new(context, sc_type_node | sc_type_const | sc_type_link);
  sc_addr const lang_ru = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_addr edge1_1 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, key_sc_element_1, target);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_key_sc_element, edge1_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_sc_definition, key_sc_element_1);
  sc_addr edge2_1 = sc_memory_arc_new(context,
                                      sc_type_arc_common | sc_type_const, sc_text_translation_1, key_sc_element_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_sc_text_translation_value, edge2_1);


  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sc_text_translation_1, text_link_1);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, lang_ru, text_link_1);

  sc_addr const key_sc_element_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const sc_text_translation_2 = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const text_link_2 = sc_memory_node_new(context, sc_type_node | sc_type_const | sc_type_link);

  sc_addr edge1_2 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, key_sc_element_2, target);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_key_sc_element, edge1_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_sc_explanation, key_sc_element_2);
  sc_addr edge2_2 = sc_memory_arc_new(context,
                                      sc_type_arc_common | sc_type_const, sc_text_translation_2, key_sc_element_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_nrel_sc_text_translation_value, edge2_2);


  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sc_text_translation_2, text_link_2);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, lang_ru, text_link_2);


  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr edge3 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, target);
  sc_addr edge4 = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, lang_ru);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edge3);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_2, edge4);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_search_tooltip, question);
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
  EXPECT_FALSE(sc_iterator3_next(it3));

  EXPECT_TRUE(sc_helper_check_arc(
                  context,
                  sc_iterator5_value(it5, 2),
                  text_link_1,
                  sc_type_arc_pos_const_perm
                  ));

  EXPECT_FALSE(sc_helper_check_arc(
                  context,
                  sc_iterator5_value(it5, 2),
                  text_link_2,
                  sc_type_arc_pos_const_perm
                  ));

  sc_iterator3_free(it3);
  sc_iterator5_free(it5);

  sc_memory_context_free(context);
  sc_module_shutdown();
}

}