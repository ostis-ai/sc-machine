

#include "search_nodes.h"
#include "search_keynodes.h"
#include "search_utils.h"
#include "search.h"

#include "sc-core/sc_helper.h"
#include "sc-core/sc_memory_headers.h"


sc_result agent_search_all_nodes_in_set(const sc_event * event, sc_addr arg)
{
  sc_addr question, answer;
  sc_iterator3 *it_argument, *it_over_set_elements;

  if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
    return SC_RESULT_ERROR_INVALID_PARAMS;

  if (sc_helper_check_arc(
          s_default_ctx,
          keynode_question_search_all_nodes_in_set,
          question,
          sc_type_arc_pos_const_perm
          ) == SC_FALSE)
    return SC_RESULT_ERROR_INVALID_TYPE;

  answer = create_answer_node();

  it_argument = sc_iterator3_f_a_a_new(
      s_default_ctx,
      question,
      sc_type_arc_pos_const_perm,
      sc_type_node
      );
  if (sc_iterator3_next(it_argument) == SC_TRUE)
  {
    it_over_set_elements = sc_iterator3_f_a_a_new(
        s_default_ctx,
        sc_iterator3_value(it_argument, 2),
        sc_type_arc_pos_const_perm,
        sc_type_node
        );
    while (sc_iterator3_next(it_over_set_elements) == SC_TRUE)
      appendIntoAnswer(answer, sc_iterator3_value(it_over_set_elements, 2));
  }

  connect_answer_to_question(question, answer);
  finish_question(question);

  return SC_RESULT_OK;
}
