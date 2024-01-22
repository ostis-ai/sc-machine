#include <gtest/gtest.h>

#include "sc_test.hpp"

extern "C"
{
#include "sc-search/search_keynodes.h"
#include "sc-utils/utils.h"
#include "sc-utils/utils_keynodes.h"
#include "sc-utils/utils_erase_elements.h"
}

TEST_F(ScMemoryTest, erase_elements_success)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr const structAddr = sc_memory_node_new(context, sc_type_node_struct | sc_type_const);

  search_keynodes_initialize(context, structAddr);

  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  sc_addr const testAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edgeAddr = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edgeAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, setAddr, testAddr);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_erase_elements, question);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_initiated, question);

  sleep(2);

  sc_iterator3 * it = sc_iterator3_f_a_a_new(context, setAddr, 0, 0);
  EXPECT_FALSE(sc_iterator3_next(it));
  sc_iterator3_free(it);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, erase_elements_from_init_struct)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr const structAddr = sc_memory_node_new(context, sc_type_node_struct | sc_type_const);

  search_keynodes_initialize(context, structAddr);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  sc_addr const testAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edgeAddr = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edgeAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, setAddr, testAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, structAddr, testAddr);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_erase_elements, question);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_initiated, question);

  sleep(2);

  sc_iterator3 * it = sc_iterator3_f_a_a_new(context, setAddr, 0, 0);
  EXPECT_TRUE(sc_iterator3_next(it));
  EXPECT_FALSE(sc_iterator3_next(it));
  sc_iterator3_free(it);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, erase_elements_self_erase)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr const structAddr = sc_memory_node_new(context, sc_type_node_struct | sc_type_const);

  search_keynodes_initialize(context, structAddr);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edgeAddr = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edgeAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, setAddr, question);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_erase_elements, question);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_initiated, question);

  sleep(2);

  sc_iterator3 * it = sc_iterator3_f_a_f_new(context, setAddr, 0, question);
  EXPECT_TRUE(sc_iterator3_next(it));
  sc_iterator3_free(it);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, erase_elements_erase_keynode)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr const structAddr = sc_memory_node_new(context, sc_type_node_struct | sc_type_const);

  search_keynodes_initialize(context, structAddr);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  sc_addr const testAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_addr const question = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edgeAddr = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, question, setAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_rrel_1, edgeAddr);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, setAddr, testAddr);

  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_erase_elements, question);
  sc_memory_arc_new(context, sc_type_arc_pos_const_perm, keynode_question_initiated, question);

  sleep(2);

  sc_iterator3 * it = sc_iterator3_f_a_a_new(context, setAddr, 0, 0);
  EXPECT_FALSE(sc_iterator3_next(it));
  sc_iterator3_free(it);

  sc_module_shutdown();
}
