#include <sc-memory/test/sc_test.hpp>

extern "C"
{
#include "utils.h"
#include "utils_keynodes.h"
#include "utils_erase_elements.h"
}

TEST_F(ScMemoryTest, erase_elements_success)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr const structAddr = sc_memory_node_new(context, sc_type_node_structure | sc_type_const);

  sc_common_keynodes_initialize(context, structAddr);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  sc_addr const testAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_addr const action = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edgeAddr = sc_memory_arc_new(context, sc_type_const_perm_pos_arc, action, setAddr);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_rrel_1, edgeAddr);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, setAddr, testAddr);

  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_erase_elements, action);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_initiated, action);

  sleep(2);

  sc_iterator3 * it = sc_iterator3_f_a_a_new(context, setAddr, 0, 0);
  EXPECT_FALSE(sc_iterator3_next(it));
  sc_iterator3_free(it);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, erase_elements_from_init_struct)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr const structAddr = sc_memory_node_new(context, sc_type_node_structure | sc_type_const);

  sc_common_keynodes_initialize(context, structAddr);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  sc_addr const testAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_addr const action = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edgeAddr = sc_memory_arc_new(context, sc_type_const_perm_pos_arc, action, setAddr);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_rrel_1, edgeAddr);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, setAddr, testAddr);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, structAddr, testAddr);

  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_erase_elements, action);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_initiated, action);

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

  sc_addr const structAddr = sc_memory_node_new(context, sc_type_node_structure | sc_type_const);

  sc_common_keynodes_initialize(context, structAddr);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  sc_addr const action = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edgeAddr = sc_memory_arc_new(context, sc_type_const_perm_pos_arc, action, setAddr);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_rrel_1, edgeAddr);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, setAddr, action);

  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_erase_elements, action);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_initiated, action);

  sleep(2);

  sc_iterator3 * it = sc_iterator3_f_a_f_new(context, setAddr, 0, action);
  EXPECT_TRUE(sc_iterator3_next(it));
  sc_iterator3_free(it);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, erase_elements_erase_keynode)
{
  sc_memory_context * context = m_ctx->GetRealContext();

  sc_addr const structAddr = sc_memory_node_new(context, sc_type_node_structure | sc_type_const);

  sc_common_keynodes_initialize(context, structAddr);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  sc_addr const testAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);

  sc_addr const action = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const setAddr = sc_memory_node_new(context, sc_type_node | sc_type_const);
  sc_addr const edgeAddr = sc_memory_arc_new(context, sc_type_const_perm_pos_arc, action, setAddr);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_rrel_1, edgeAddr);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, setAddr, testAddr);

  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_erase_elements, action);
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_initiated, action);

  sleep(2);

  sc_iterator3 * it = sc_iterator3_f_a_a_new(context, setAddr, 0, 0);
  EXPECT_FALSE(sc_iterator3_next(it));
  sc_iterator3_free(it);

  sc_module_shutdown();
}
