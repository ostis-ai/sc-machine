/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "search.h"
#include "search_agents.h"
#include "search_keynodes.h"

#include "sc-core/sc_keynodes.h"
#include "sc-core/sc_memory_headers.h"

sc_memory_context * s_default_ctx = 0;

sc_event_subscription * event_action_search_all_outgoing_arcs;
sc_event_subscription * event_action_search_all_incoming_arcs;
sc_event_subscription * event_action_search_all_outgoing_arcs_with_rel;
sc_event_subscription * event_action_search_all_incoming_arcs_with_rel;
sc_event_subscription * event_action_search_full_semantic_neighborhood;
sc_event_subscription * event_action_search_all_subclasses_in_quasybinary_relation;
sc_event_subscription * event_action_search_all_superclasses_in_quasybinary_relation;
sc_event_subscription * event_action_search_decomposition;
sc_event_subscription * event_action_search_all_identifiers;
sc_event_subscription * event_action_search_all_identified_elements;
sc_event_subscription * event_action_search_links_of_relation_connected_with_element;

// --------------------- Module ------------------------

sc_result sc_module_initialize_with_init_memory_generated_structure(sc_addr const init_memory_generated_structure)
{
  s_default_ctx = s_memory_default_ctx;

  if (search_keynodes_initialize(s_default_ctx, init_memory_generated_structure) != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  event_action_search_all_outgoing_arcs = sc_event_subscription_new(
      s_default_ctx,
      keynode_action_initiated,
      sc_event_after_generate_outgoing_arc_addr,
      0,
      agent_search_all_const_pos_outgoing_arc,
      0);
  if (event_action_search_all_outgoing_arcs == null_ptr)
    return SC_RESULT_ERROR;

  event_action_search_all_incoming_arcs = sc_event_subscription_new(
      s_default_ctx,
      keynode_action_initiated,
      sc_event_after_generate_outgoing_arc_addr,
      0,
      agent_search_all_const_pos_incoming_arc,
      0);
  if (event_action_search_all_incoming_arcs == null_ptr)
    return SC_RESULT_ERROR;

  event_action_search_all_outgoing_arcs_with_rel = sc_event_subscription_new(
      s_default_ctx,
      keynode_action_initiated,
      sc_event_after_generate_outgoing_arc_addr,
      0,
      agent_search_all_const_pos_outgoing_arc_with_rel,
      0);
  if (event_action_search_all_incoming_arcs == null_ptr)
    return SC_RESULT_ERROR;

  event_action_search_all_incoming_arcs_with_rel = sc_event_subscription_new(
      s_default_ctx,
      keynode_action_initiated,
      sc_event_after_generate_outgoing_arc_addr,
      0,
      agent_search_all_const_pos_incoming_arc_with_rel,
      0);
  if (event_action_search_all_incoming_arcs == null_ptr)
    return SC_RESULT_ERROR;

  event_action_search_full_semantic_neighborhood = sc_event_subscription_new(
      s_default_ctx,
      keynode_action_initiated,
      sc_event_after_generate_outgoing_arc_addr,
      0,
      agent_search_full_semantic_neighborhood,
      0);
  if (event_action_search_full_semantic_neighborhood == null_ptr)
    return SC_RESULT_ERROR;

  event_action_search_decomposition = sc_event_subscription_new(
      s_default_ctx,
      keynode_action_initiated,
      sc_event_after_generate_outgoing_arc_addr,
      0,
      agent_search_decomposition,
      0);
  if (event_action_search_decomposition == null_ptr)
    return SC_RESULT_ERROR;

  event_action_search_all_identifiers = sc_event_subscription_new(
      s_default_ctx,
      keynode_action_initiated,
      sc_event_after_generate_outgoing_arc_addr,
      0,
      agent_search_all_identifiers,
      0);
  if (event_action_search_all_identifiers == null_ptr)
    return SC_RESULT_ERROR;

  event_action_search_all_identified_elements = sc_event_subscription_new(
      s_default_ctx,
      keynode_action_initiated,
      sc_event_after_generate_outgoing_arc_addr,
      0,
      agent_search_all_identified_elements,
      0);
  if (event_action_search_all_identified_elements == null_ptr)
    return SC_RESULT_ERROR;

  event_action_search_all_subclasses_in_quasybinary_relation = sc_event_subscription_new(
      s_default_ctx,
      keynode_action_initiated,
      sc_event_after_generate_outgoing_arc_addr,
      0,
      agent_search_all_subclasses_in_quasybinary_relation,
      0);
  if (event_action_search_all_subclasses_in_quasybinary_relation == null_ptr)
    return SC_RESULT_ERROR;

  event_action_search_all_superclasses_in_quasybinary_relation = sc_event_subscription_new(
      s_default_ctx,
      keynode_action_initiated,
      sc_event_after_generate_outgoing_arc_addr,
      0,
      agent_search_all_superclasses_in_quasybinary_relation,
      0);
  if (event_action_search_all_superclasses_in_quasybinary_relation == null_ptr)
    return SC_RESULT_ERROR;

  event_action_search_links_of_relation_connected_with_element = sc_event_subscription_new(
      s_default_ctx,
      keynode_action_initiated,
      sc_event_after_generate_outgoing_arc_addr,
      0,
      agent_search_links_of_relation_connected_with_element,
      0);
  if (event_action_search_links_of_relation_connected_with_element == null_ptr)
    return SC_RESULT_ERROR;

  return SC_RESULT_OK;
}

_SC_EXT_EXTERN sc_uint32 sc_module_load_priority()
{
  return 2;
}

sc_result sc_module_shutdown()
{
  if (event_action_search_all_outgoing_arcs)
    sc_event_subscription_destroy(event_action_search_all_outgoing_arcs);
  if (event_action_search_all_incoming_arcs)
    sc_event_subscription_destroy(event_action_search_all_incoming_arcs);
  if (event_action_search_all_outgoing_arcs_with_rel)
    sc_event_subscription_destroy(event_action_search_all_outgoing_arcs_with_rel);
  if (event_action_search_all_incoming_arcs_with_rel)
    sc_event_subscription_destroy(event_action_search_all_incoming_arcs_with_rel);
  if (event_action_search_full_semantic_neighborhood)
    sc_event_subscription_destroy(event_action_search_full_semantic_neighborhood);
  if (event_action_search_all_identified_elements)
    sc_event_subscription_destroy(event_action_search_all_identified_elements);
  if (event_action_search_all_identifiers)
    sc_event_subscription_destroy(event_action_search_all_identifiers);
  if (event_action_search_decomposition)
    sc_event_subscription_destroy(event_action_search_decomposition);
  if (event_action_search_all_subclasses_in_quasybinary_relation)
    sc_event_subscription_destroy(event_action_search_all_subclasses_in_quasybinary_relation);
  if (event_action_search_all_superclasses_in_quasybinary_relation)
    sc_event_subscription_destroy(event_action_search_all_superclasses_in_quasybinary_relation);
  if (event_action_search_links_of_relation_connected_with_element)
    sc_event_subscription_destroy(event_action_search_links_of_relation_connected_with_element);

  return SC_RESULT_OK;
}
