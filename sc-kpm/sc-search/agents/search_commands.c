
#include "search_commands.h"
#include "search_keynodes.h"
#include "search_utils.h"
#include "search.h"

#include "sc-core/sc_helper.h"
#include "sc-core/sc_memory_headers.h"


void process_command_decompositions(sc_addr command, sc_addr answer, sc_addr checked_commands);

sc_result agent_search_atomic_commands(const sc_event * event, sc_addr arg)
{
  sc_addr question, answer, checked_commands;

  if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
    return SC_RESULT_ERROR_INVALID_PARAMS;

  if (sc_helper_check_arc(
          s_default_ctx,
          keynode_question_search_atomic_commands,
          question,
          sc_type_arc_pos_const_perm
          ) == SC_FALSE)
    return SC_RESULT_ERROR_INVALID_TYPE;

  answer = create_answer_node();

  checked_commands = sc_memory_node_new(s_default_ctx, sc_type_node | sc_type_const);

  process_command_decompositions(keynode_ui_main_menu, answer, checked_commands);

  sc_memory_element_free(s_default_ctx, checked_commands);

  connect_answer_to_question(question, answer);
  finish_question(question);

  return SC_RESULT_OK;
}

void process_command_decompositions(sc_addr command, sc_addr answer, sc_addr checked_commands)
{
  sc_iterator3 *it_over_decomposition_elements;
  sc_iterator5 *it_over_command_decompositions;

  it_over_command_decompositions = sc_iterator5_a_a_f_a_f_new(
      s_default_ctx,
      sc_type_node | sc_type_const,
      sc_type_arc_common | sc_type_const,
      command,
      sc_type_arc_pos_const_perm,
      keynode_nrel_ui_commands_decomposition
  );

  while (sc_iterator5_next(it_over_command_decompositions) == SC_TRUE)
  {
    it_over_decomposition_elements = sc_iterator3_f_a_a_new(
        s_default_ctx,
        sc_iterator5_value(it_over_command_decompositions, 0),
        sc_type_arc_pos_const_perm,
        sc_type_node | sc_type_const
        );

    while (sc_iterator3_next(it_over_decomposition_elements) == SC_TRUE)
    {
      sc_addr command_child = sc_iterator3_value(it_over_decomposition_elements, 2);
      if (sc_helper_check_arc(
              s_default_ctx,
              keynode_ui_user_command_class_atom,
              command_child,
              sc_type_arc_pos_const_perm
              ) == SC_TRUE)
        appendIntoAnswer(answer, command_child);

      if (sc_helper_check_arc(
              s_default_ctx,
              checked_commands,
              command_child,
              sc_type_arc_pos_const_perm
              ) == SC_FALSE)
      {
        sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, checked_commands, command_child);
        process_command_decompositions(command_child, answer, checked_commands);
      }
    }

    sc_iterator3_free(it_over_decomposition_elements);
  }

  sc_iterator5_free(it_over_command_decompositions);
}
