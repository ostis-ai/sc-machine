/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "search.h"
#include "sc_helper.h"
#include "sc_memory_headers.h"
#include "search_agents.h"
#include "search_keynodes.h"

sc_memory_context * s_default_ctx = 0;


sc_event *event_question_search_all_output_arcs;
sc_event *event_question_search_all_input_arcs;
sc_event *event_question_search_all_output_arcs_with_rel;
sc_event *event_question_search_all_input_arcs_with_rel;
sc_event *event_question_search_full_semantic_neighborhood;
sc_event *event_question_search_all_subclasses_in_quasybinary_relation;
sc_event *event_question_search_all_superclasses_in_quasybinary_relation;
sc_event *event_question_search_decomposition;
sc_event *event_question_search_all_identifiers;
sc_event *event_question_search_all_identified_elements;
sc_event *event_question_search_links_of_relation_connected_with_element;

// --------------------- Module ------------------------

_SC_EXT_EXTERN sc_result initialize()
{
    s_default_ctx = sc_memory_context_new(sc_access_lvl_make_min);

    if (search_keynodes_initialize() != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    event_question_search_all_output_arcs = sc_event_new(s_default_ctx, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_search_all_const_pos_output_arc, 0);
    if (event_question_search_all_output_arcs == null_ptr)
        return SC_RESULT_ERROR;

    event_question_search_all_input_arcs = sc_event_new(s_default_ctx, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_search_all_const_pos_input_arc, 0);
    if (event_question_search_all_input_arcs == null_ptr)
        return SC_RESULT_ERROR;

    event_question_search_all_output_arcs_with_rel = sc_event_new(s_default_ctx, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_search_all_const_pos_output_arc_with_rel, 0);
    if (event_question_search_all_input_arcs == null_ptr)
        return SC_RESULT_ERROR;

    event_question_search_all_input_arcs_with_rel = sc_event_new(s_default_ctx, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_search_all_const_pos_input_arc_with_rel, 0);
    if (event_question_search_all_input_arcs == null_ptr)
        return SC_RESULT_ERROR;

    event_question_search_full_semantic_neighborhood = sc_event_new(s_default_ctx, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_search_full_semantic_neighborhood, 0);
    if (event_question_search_full_semantic_neighborhood == null_ptr)
        return SC_RESULT_ERROR;

    event_question_search_decomposition = sc_event_new(s_default_ctx, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_search_decomposition, 0);
    if (event_question_search_decomposition == null_ptr)
        return SC_RESULT_ERROR;

    event_question_search_all_identifiers = sc_event_new(s_default_ctx, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_search_all_identifiers, 0);
    if (event_question_search_all_identifiers == null_ptr)
        return SC_RESULT_ERROR;

    event_question_search_all_identified_elements = sc_event_new(s_default_ctx, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_search_all_identified_elements, 0);
    if (event_question_search_all_identified_elements == null_ptr)
        return SC_RESULT_ERROR;

    event_question_search_all_subclasses_in_quasybinary_relation = sc_event_new(s_default_ctx, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_search_all_subclasses_in_quasybinary_relation, 0);
    if (event_question_search_all_subclasses_in_quasybinary_relation == null_ptr)
        return SC_RESULT_ERROR;

    event_question_search_all_superclasses_in_quasybinary_relation = sc_event_new(s_default_ctx, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_search_all_superclasses_in_quasybinary_relation, 0);
    if (event_question_search_all_superclasses_in_quasybinary_relation == null_ptr)
        return SC_RESULT_ERROR;

    event_question_search_links_of_relation_connected_with_element = sc_event_new(s_default_ctx, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_search_links_of_relation_connected_with_element, 0);
    if (event_question_search_links_of_relation_connected_with_element == null_ptr)
        return SC_RESULT_ERROR;

    return SC_RESULT_OK;
}

_SC_EXT_EXTERN sc_result shutdown()
{
    if (event_question_search_all_output_arcs)
        sc_event_destroy(event_question_search_all_output_arcs);
    if (event_question_search_all_input_arcs)
        sc_event_destroy(event_question_search_all_input_arcs);
    if (event_question_search_all_output_arcs_with_rel)
        sc_event_destroy(event_question_search_all_output_arcs_with_rel);
    if (event_question_search_all_input_arcs_with_rel)
        sc_event_destroy(event_question_search_all_input_arcs_with_rel);
    if (event_question_search_full_semantic_neighborhood)
        sc_event_destroy(event_question_search_full_semantic_neighborhood);
    if (event_question_search_all_identified_elements)
        sc_event_destroy(event_question_search_all_identified_elements);
    if (event_question_search_all_identifiers)
        sc_event_destroy(event_question_search_all_identifiers);
    if (event_question_search_decomposition)
        sc_event_destroy(event_question_search_decomposition);
    if (event_question_search_all_subclasses_in_quasybinary_relation)
        sc_event_destroy(event_question_search_all_subclasses_in_quasybinary_relation);
    if (event_question_search_all_superclasses_in_quasybinary_relation)
        sc_event_destroy(event_question_search_all_superclasses_in_quasybinary_relation);
    if (event_question_search_links_of_relation_connected_with_element)
        sc_event_destroy(event_question_search_links_of_relation_connected_with_element);

    sc_memory_context_free(s_default_ctx);

    return SC_RESULT_OK;
}
