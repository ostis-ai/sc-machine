
#include "search_tooltip.h"
#include "search_keynodes.h"
#include "search_utils.h"
#include "search.h"

#include "sc-core/sc_helper.h"
#include "sc-core/sc_memory_headers.h"

#include <stdio.h>

sc_bool find_tooltip(sc_addr node, sc_addr lang, sc_addr class, sc_addr * tooltip);

sc_result agent_search_tooltip(const sc_event * event, sc_addr arg)
{
  printf("start agent_search_tooltip\n");
  sc_addr question, answer, tooltip;
  sc_iterator5 *it_rrel_argument;
  sc_addr node, lang;
  sc_addr possible_classes[] = {keynode_sc_definition, keynode_sc_explanation, keynode_sc_note};
  int amount_of_possible_classes = sizeof(possible_classes) / sizeof(*possible_classes);
  sc_bool tooltip_is_found = SC_FALSE;

  if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
    return SC_RESULT_ERROR_INVALID_PARAMS;

  if (sc_helper_check_arc(
          s_default_ctx,
          keynode_question_search_tooltip,
          question,
          sc_type_arc_pos_const_perm
          ) == SC_FALSE)
    return SC_RESULT_ERROR_INVALID_TYPE;

  answer = create_answer_node();

  it_rrel_argument = sc_iterator5_f_a_a_a_f_new(
      s_default_ctx,
      question,
      sc_type_arc_pos_const_perm,
      sc_type_node| sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_rrel_1
      );

  if (sc_iterator5_next(it_rrel_argument) == SC_TRUE)
    node = sc_iterator5_value(it_rrel_argument, 2);
  else
    return SC_RESULT_ERROR_INVALID_PARAMS;

  sc_iterator5_free(it_rrel_argument);

  it_rrel_argument = sc_iterator5_f_a_a_a_f_new(
      s_default_ctx,
      question,
      sc_type_arc_pos_const_perm,
      sc_type_node| sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_rrel_2
      );

    if (sc_iterator5_next(it_rrel_argument) == SC_TRUE)
      lang = sc_iterator5_value(it_rrel_argument, 2);
    else
      return SC_RESULT_ERROR_INVALID_PARAMS;

    sc_iterator5_free(it_rrel_argument);

    for (int i = 0; i < amount_of_possible_classes && tooltip_is_found == SC_FALSE; i++)
      tooltip_is_found = find_tooltip(node, lang, possible_classes[i], &tooltip);

    if (tooltip_is_found == SC_TRUE)
      appendIntoAnswer(answer, tooltip);


    connect_answer_to_question(question, answer);
    finish_question(question);

    printf("finish agent_search_tooltip\n");
    return SC_RESULT_OK;
}

sc_bool find_tooltip(sc_addr node, sc_addr lang, sc_addr class, sc_addr * tooltip)
{
  sc_addr key_sc_element, sc_text_translation_value;
  sc_iterator3 *it_over_text_links;
  sc_iterator5 *it_over_key_sc_elements, *it_over_sc_text_translation_values;

  it_over_key_sc_elements = sc_iterator5_a_a_f_a_f_new(
      s_default_ctx,
      sc_type_node| sc_type_const,
      sc_type_arc_pos_const_perm,
      node,
      sc_type_arc_pos_const_perm,
      keynode_rrel_key_sc_element
  );

  while (sc_iterator5_next(it_over_key_sc_elements))
  {
    key_sc_element = sc_iterator5_value(it_over_key_sc_elements, 0);
    if (sc_helper_check_arc(
        s_default_ctx,
        class,
        key_sc_element,
        sc_type_arc_pos_const_perm
        ) == SC_TRUE)
    {
      it_over_sc_text_translation_values = sc_iterator5_a_a_f_a_f_new(
          s_default_ctx,
          sc_type_node| sc_type_const,
          sc_type_arc_common | sc_type_const,
          key_sc_element,
          sc_type_arc_pos_const_perm,
          keynode_nrel_sc_text_translation);
      while (sc_iterator5_next(it_over_sc_text_translation_values))
      {
        sc_text_translation_value = sc_iterator5_value(it_over_sc_text_translation_values, 0);
        it_over_text_links = sc_iterator3_f_a_a_new(
            s_default_ctx,
            sc_text_translation_value,
            sc_type_arc_pos_const_perm,
            sc_type_node | sc_type_const | sc_type_link
            );
        while (sc_iterator3_next(it_over_text_links))
        {
          if (sc_helper_check_arc(
                  s_default_ctx,
                  lang,
                  sc_iterator3_value(it_over_text_links, 2),
                  sc_type_arc_pos_const_perm
                  ) == SC_TRUE)
          {
            *tooltip = sc_iterator3_value(it_over_text_links, 2);
            sc_iterator3_free(it_over_text_links);
            sc_iterator5_free(it_over_key_sc_elements);
            sc_iterator5_free(it_over_sc_text_translation_values);
            return SC_TRUE;
          }
        }
        sc_iterator3_free(it_over_text_links);
      }
      sc_iterator5_free(it_over_sc_text_translation_values);
    }
  }
  sc_iterator5_free(it_over_key_sc_elements);
  return SC_FALSE;
}

