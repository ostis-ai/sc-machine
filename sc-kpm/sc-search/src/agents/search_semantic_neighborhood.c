/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "search_semantic_neighborhood.h"
#include "search_keynodes.h"
#include "search.h"

#include <stdio.h>

#include <sc-core/sc_memory_headers.h>

#include <sc-common/sc_utils.h>

void search_translation(sc_addr elem, sc_addr result, sc_bool sys_off)
{
  sc_iterator5 * it5;
  sc_iterator3 *it3, *it4;
  sc_bool found = SC_FALSE;

  // iterate translations of sc-element
  it5 = sc_iterator5_a_a_f_a_f_new(
      s_default_ctx,
      sc_type_node | sc_type_const,
      sc_type_const_common_arc,
      elem,
      sc_type_const_perm_pos_arc,
      keynode_nrel_translation);
  while (sc_iterator5_next(it5) == SC_TRUE)
  {
    found = SC_TRUE;

    if (sys_off == SC_TRUE
        && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 0))
            || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 1))
            || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 3))))
      continue;

    appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 0));
    appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 1));
    appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 2));
    appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 3));

    // iterate translation sc-links
    it3 = sc_iterator3_f_a_a_new(s_default_ctx, sc_iterator5_value(it5, 0), sc_type_const_perm_pos_arc, 0);
    while (sc_iterator3_next(it3) == SC_TRUE)
    {
      if (sys_off == SC_TRUE
          && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it3, 1))
              || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it3, 2))))
        continue;

      // iterate incoming sc-arcs for link
      it4 = sc_iterator3_a_a_f_new(s_default_ctx, sc_type_node, sc_type_const_perm_pos_arc, sc_iterator3_value(it3, 2));
      while (sc_iterator3_next(it4) == SC_TRUE)
      {
        if (sys_off == SC_TRUE
            && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it4, 1))
                || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it4, 0))))
          continue;
        if (sc_helper_check_arc(
                s_default_ctx, keynode_languages, sc_iterator3_value(it4, 0), sc_type_const_perm_pos_arc)
            == SC_TRUE)
        {
          appendIntoResult(s_default_ctx, result, sc_iterator3_value(it4, 0));
          appendIntoResult(s_default_ctx, result, sc_iterator3_value(it4, 1));
        }
      }
      sc_iterator3_free(it4);

      // iterate incoming sc-arcs for arc
      it4 = sc_iterator3_a_a_f_new(s_default_ctx, sc_type_node, sc_type_const_perm_pos_arc, sc_iterator3_value(it3, 1));
      while (sc_iterator3_next(it4) == SC_TRUE)
      {
        if (sys_off == SC_TRUE
            && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it4, 0))
                || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it4, 1))))
          continue;

        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it4, 0));
        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it4, 1));
      }
      sc_iterator3_free(it4);

      appendIntoResult(s_default_ctx, result, sc_iterator3_value(it3, 1));
      appendIntoResult(s_default_ctx, result, sc_iterator3_value(it3, 2));
    }
    sc_iterator3_free(it3);
  }
  sc_iterator5_free(it5);

  if (found == SC_TRUE)
  {
    appendIntoResult(s_default_ctx, result, keynode_nrel_translation);
  }
}

void search_arc_components(sc_addr elem, sc_addr result, sc_bool sys_off)
{
  sc_type type;
  sc_addr begin, end;

  if (SC_RESULT_OK != sc_memory_get_element_type(s_default_ctx, elem, &type))
    return;
  if (!(type & ~sc_type_node))
    return;
  if (SC_RESULT_OK != sc_memory_get_arc_begin(s_default_ctx, elem, &begin))
    return;
  if (SC_RESULT_OK != sc_memory_get_arc_end(s_default_ctx, elem, &end))
    return;

  appendIntoResult(s_default_ctx, result, begin);
  appendIntoResult(s_default_ctx, result, end);
}

void search_nonbinary_relation(sc_addr elem, sc_addr result, sc_bool sys_off)
{
  sc_iterator3 *it1, *it2, *it3;
  sc_type el_type;

  // iterate incoming sc-arcs for elem
  it1 = sc_iterator3_a_a_f_new(s_default_ctx, sc_type_node | sc_type_const, sc_type_const_perm_pos_arc, elem);
  while (sc_iterator3_next(it1) == SC_TRUE)
  {
    // if elem is a link of non-binary relation
    if (SC_TRUE
        == sc_helper_check_arc(
            s_default_ctx, keynode_nonbinary_relation, sc_iterator3_value(it1, 0), sc_type_const_perm_pos_arc))
    {
      if (sys_off == SC_TRUE
          && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it1, 0))
              || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it1, 1))))
        continue;

      // iterate other elements of link
      it2 = sc_iterator3_f_a_a_new(s_default_ctx, elem, sc_type_const_perm_pos_arc, sc_type_node | sc_type_const);
      while (sc_iterator3_next(it2) == SC_TRUE)
      {
        if (sys_off == SC_TRUE
            && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it2, 1))
                || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it2, 2))))
          continue;

        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it2, 1));
        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it2, 2));

        search_arc_components(sc_iterator3_value(it2, 2), result, sys_off);

        // iterate attributes of link
        it3 = sc_iterator3_a_a_f_new(
            s_default_ctx, sc_type_node | sc_type_const, sc_type_const_perm_pos_arc, sc_iterator3_value(it2, 1));
        while (sc_iterator3_next(it3) == SC_TRUE)
        {
          if (sys_off == SC_TRUE
              && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it3, 0))
                  || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it3, 1))))
            continue;

          sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it3, 0), &el_type);
          if (!(el_type & (sc_type_node_no_role | sc_type_node_role)))
            continue;

          appendIntoResult(s_default_ctx, result, sc_iterator3_value(it3, 0));
          appendIntoResult(s_default_ctx, result, sc_iterator3_value(it3, 1));
        }
        sc_iterator3_free(it3);
      }
      sc_iterator3_free(it2);

      appendIntoResult(s_default_ctx, result, sc_iterator3_value(it1, 0));
      appendIntoResult(s_default_ctx, result, sc_iterator3_value(it1, 1));
    }
  }
  sc_iterator3_free(it1);
}

void search_typical_sc_neighborhood(sc_addr elem, sc_addr result, sc_bool sys_off)
{
  sc_iterator3 *it1, *it0;
  sc_iterator5 * it5;
  sc_bool found = SC_FALSE;

  // search for keynode_typical_sc_neighborhood
  it0 = sc_iterator3_a_a_f_new(s_default_ctx, sc_type_node | sc_type_const, sc_type_const_perm_pos_arc, elem);
  while (sc_iterator3_next(it0) == SC_TRUE)
  {
    if (SC_ADDR_IS_EQUAL(sc_iterator3_value(it0, 0), keynode_typical_sc_neighborhood))
    {
      found = SC_TRUE;
      // iterate incoming sc-arcs for elem
      it1 = sc_iterator3_f_a_a_new(s_default_ctx, elem, sc_type_const_perm_pos_arc, 0);
      while (sc_iterator3_next(it1) == SC_TRUE)
      {
        if (sys_off == SC_TRUE
            && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it1, 1))
                || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it1, 2))))
          continue;

        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it1, 1));
        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it1, 2));
      }
      sc_iterator3_free(it1);

      appendIntoResult(s_default_ctx, result, sc_iterator3_value(it0, 1));
      appendIntoResult(s_default_ctx, result, sc_iterator3_value(it0, 0));
      continue;
    }

    if (sys_off == SC_TRUE
        && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it0, 0))
            || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it0, 1))))
      continue;

    it5 = sc_iterator5_f_a_f_a_f_new(
        s_default_ctx,
        keynode_sc_neighborhood,
        sc_type_const_common_arc,
        sc_iterator3_value(it0, 0),
        sc_type_const_perm_pos_arc,
        keynode_nrel_strict_inclusion);
    if (sc_iterator5_next(it5) == SC_TRUE)
    {
      if (sys_off == SC_TRUE
          && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 1))
              || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 3))))
        continue;

      appendIntoResult(s_default_ctx, result, sc_iterator3_value(it0, 0));
      appendIntoResult(s_default_ctx, result, sc_iterator3_value(it0, 1));
      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 1));
      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 3));
    }
    sc_iterator5_free(it5);
  }
  sc_iterator3_free(it0);
  if (found == SC_TRUE)
  {
    appendIntoResult(s_default_ctx, result, keynode_typical_sc_neighborhood);
  }
}

void search_element_identifiers(sc_addr el, sc_addr result)
{
  sc_iterator3 * it2;
  sc_iterator5 * it5;

  // iterate all const arcs, that are no accessory, and go out from sc-element
  it5 = sc_iterator5_f_a_a_a_a_new(
      s_default_ctx,
      el,
      sc_type_const_common_arc,
      sc_type_node_link,
      sc_type_const_perm_pos_arc,
      sc_type_node | sc_type_const | sc_type_node_no_role);
  while (sc_iterator5_next(it5) == SC_TRUE)
  {
    // check if this relation is an identification
    if (sc_helper_check_arc(
            s_default_ctx, keynode_identification_relation, sc_iterator5_value(it5, 4), sc_type_const_perm_pos_arc)
        == SC_TRUE)
    {
      // iterate incoming sc-arcs for sc-link
      it2 = sc_iterator3_a_a_f_new(
          s_default_ctx, sc_type_node | sc_type_const, sc_type_const_perm_pos_arc, sc_iterator5_value(it5, 2));
      while (sc_iterator3_next(it2) == SC_TRUE)
      {
        if (sc_helper_check_arc(
                s_default_ctx, keynode_languages, sc_iterator3_value(it2, 0), sc_type_const_perm_pos_arc)
            == SC_TRUE)
        {
          appendIntoResult(s_default_ctx, result, sc_iterator3_value(it2, 0));
          appendIntoResult(s_default_ctx, result, sc_iterator3_value(it2, 1));
        }
      }
      sc_iterator3_free(it2);

      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 1));
      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 2));
      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 3));
      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 4));
    }
  }
  sc_iterator5_free(it5);
}

sc_result agent_search_full_semantic_neighborhood(sc_event_subscription const * event, sc_addr arg)
{
  sc_addr action, result;
  sc_iterator3 *it1, *it2, *it3, *it4, *it6;
  sc_iterator5 *it5, *it_order, *it_order2;
  sc_type el_type;
  sc_bool sys_off = SC_TRUE;
  sc_bool key_order_found = SC_FALSE;

  if (!sc_memory_get_arc_end(s_default_ctx, arg, &action))
    return SC_RESULT_ERROR_INVALID_PARAMS;

  // check action type
  if (sc_helper_check_arc(s_default_ctx, keynode_action_full_semantic_neighborhood, action, sc_type_const_perm_pos_arc)
      == SC_FALSE)
    return SC_RESULT_ERROR_INVALID_TYPE;

  result = create_result_node(s_default_ctx);

  // get action argument
  it1 = sc_iterator3_f_a_a_new(s_default_ctx, action, sc_type_const_perm_pos_arc, 0);
  if (sc_iterator3_next(it1) == SC_TRUE)
  {
    sc_addr const element = sc_iterator3_value(it1, 2);

    appendIntoResult(s_default_ctx, result, element);

    search_translation(element, result, sys_off);
    search_arc_components(element, result, sys_off);

    sc_iterator3 * sysElementIt3 =
        sc_iterator3_f_a_f_new(s_default_ctx, keynode_system_element, sc_type_const_perm_pos_arc, element);
    if (sc_iterator3_next(sysElementIt3) == SC_TRUE)
    {
      appendIntoResult(s_default_ctx, result, keynode_system_element);
      appendIntoResult(s_default_ctx, result, sc_iterator3_value(sysElementIt3, 1));

      search_element_identifiers(element, result);

      connect_result_to_action(s_default_ctx, action, result);
      finish_action(s_default_ctx, action);
      sc_iterator3_free(sysElementIt3);
      return SC_RESULT_OK;
    }
    sc_iterator3_free(sysElementIt3);

    // iterate incoming sc-arcs
    it2 = sc_iterator3_a_a_f_new(s_default_ctx, 0, 0, sc_iterator3_value(it1, 2));
    while (sc_iterator3_next(it2) == SC_TRUE)
    {
      if (sys_off == SC_TRUE
          && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it2, 0))
              || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it2, 1))))
        continue;

      appendIntoResult(s_default_ctx, result, sc_iterator3_value(it2, 0));
      appendIntoResult(s_default_ctx, result, sc_iterator3_value(it2, 1));

      search_arc_components(sc_iterator3_value(it2, 0), result, sys_off);

      // iterate incoming sc-arcs into found arc, to find relations
      it3 = sc_iterator3_a_a_f_new(s_default_ctx, sc_type_node, sc_type_const_perm_pos_arc, sc_iterator3_value(it2, 1));
      while (sc_iterator3_next(it3) == SC_TRUE)
      {
        sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it3, 0), &el_type);
        if (!(el_type & (sc_type_node_no_role | sc_type_node_role)))
          continue;

        if (sys_off == SC_TRUE
            && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it3, 1))
                || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it3, 0))))
          continue;

        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it3, 0));
        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it3, 1));

        search_arc_components(sc_iterator3_value(it3, 0), result, sys_off);

        // search typical sc-neighborhood if necessary
        if (SC_ADDR_IS_EQUAL(keynode_rrel_key_sc_element, sc_iterator3_value(it3, 0)))
        {
          search_typical_sc_neighborhood(sc_iterator3_value(it2, 0), result, sys_off);
          search_translation(sc_iterator3_value(it2, 0), result, sys_off);
        }

        // check if it's a quasy binary relation
        if (sc_helper_check_arc(
                s_default_ctx, keynode_quasybinary_relation, sc_iterator3_value(it3, 0), sc_type_const_perm_pos_arc)
            == SC_TRUE)
        {
          // iterate elements of relation
          it4 = sc_iterator3_f_a_a_new(s_default_ctx, sc_iterator3_value(it2, 0), sc_type_const_perm_pos_arc, 0);
          while (sc_iterator3_next(it4) == SC_TRUE)
          {
            if (sys_off == SC_TRUE
                && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it4, 1))
                    || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it4, 2))))
              continue;

            appendIntoResult(s_default_ctx, result, sc_iterator3_value(it4, 1));
            appendIntoResult(s_default_ctx, result, sc_iterator3_value(it4, 2));

            search_arc_components(sc_iterator3_value(it4, 2), result, sys_off);

            // iterate order relations between elements
            it_order = sc_iterator5_f_a_a_a_a_new(
                s_default_ctx,
                sc_iterator3_value(it4, 2),
                sc_type_const_common_arc,
                sc_type_node | sc_type_const,
                sc_type_const_perm_pos_arc,
                sc_type_node | sc_type_const);
            while (sc_iterator5_next(it_order) == SC_TRUE)
            {
              if (sys_off == SC_TRUE
                  && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it_order, 1))
                      || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it_order, 2))
                      || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it_order, 3))
                      || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it_order, 4))))
                continue;

              if (SC_FALSE
                  == sc_helper_check_arc(
                      s_default_ctx,
                      keynode_order_relation,
                      sc_iterator5_value(it_order, 4),
                      sc_type_const_perm_pos_arc))
                continue;
              if (SC_FALSE
                  == sc_helper_check_arc(
                      s_default_ctx,
                      sc_iterator3_value(it2, 0),
                      sc_iterator5_value(it_order, 2),
                      sc_type_const_perm_pos_arc))
                continue;

              appendIntoResult(s_default_ctx, result, sc_iterator5_value(it_order, 1));
              appendIntoResult(s_default_ctx, result, sc_iterator5_value(it_order, 2));
              appendIntoResult(s_default_ctx, result, sc_iterator5_value(it_order, 3));
              appendIntoResult(s_default_ctx, result, sc_iterator5_value(it_order, 4));
            }
            sc_iterator5_free(it_order);

            // iterate roles of element in link
            it6 = sc_iterator3_a_a_f_new(
                s_default_ctx, sc_type_node | sc_type_const, sc_type_const_perm_pos_arc, sc_iterator3_value(it4, 1));
            while (sc_iterator3_next(it6) == SC_TRUE)
            {
              sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it6, 0), &el_type);
              if (!(el_type & sc_type_node_role))
                continue;

              if (sys_off == SC_TRUE
                  && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it6, 0))
                      || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it6, 1))))
                continue;

              appendIntoResult(s_default_ctx, result, sc_iterator3_value(it6, 0));
              appendIntoResult(s_default_ctx, result, sc_iterator3_value(it6, 1));

              search_arc_components(sc_iterator3_value(it6, 0), result, sys_off);
            }
            sc_iterator3_free(it6);
          }
          sc_iterator3_free(it4);
        }
      }
      sc_iterator3_free(it3);

      // search all parents in quasybinary relation
      it5 = sc_iterator5_f_a_a_a_a_new(
          s_default_ctx,
          sc_iterator3_value(it2, 0),
          sc_type_const_common_arc,
          sc_type_node | sc_type_const,
          sc_type_const_perm_pos_arc,
          sc_type_node | sc_type_const);
      while (sc_iterator5_next(it5) == SC_TRUE)
      {
        // check if it's a quasy binary relation
        if (sc_helper_check_arc(
                s_default_ctx, keynode_quasybinary_relation, sc_iterator5_value(it5, 4), sc_type_const_perm_pos_arc)
            == SC_TRUE)
        {
          if (sys_off == SC_TRUE
              && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 1))
                  || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 2))
                  || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 3))
                  || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 4))))
            continue;

          appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 1));
          appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 2));
          appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 3));
          appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 4));

          search_arc_components(sc_iterator5_value(it5, 2), result, sys_off);
        }
      }
      sc_iterator5_free(it5);

      // search non-binary relation link
      search_nonbinary_relation(sc_iterator3_value(it2, 0), result, sys_off);
    }
    sc_iterator3_free(it2);

    // iterate outgoing sc-arcs
    it2 = sc_iterator3_f_a_a_new(s_default_ctx, sc_iterator3_value(it1, 2), 0, 0);
    while (sc_iterator3_next(it2) == SC_TRUE)
    {
      if (sys_off == SC_TRUE
          && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it2, 1))
              || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it2, 2))))
        continue;

      appendIntoResult(s_default_ctx, result, sc_iterator3_value(it2, 1));
      appendIntoResult(s_default_ctx, result, sc_iterator3_value(it2, 2));

      search_arc_components(sc_iterator3_value(it2, 2), result, sys_off);

      // iterate incoming sc-arcs into found arc, to find relations
      it3 = sc_iterator3_a_a_f_new(s_default_ctx, sc_type_node, sc_type_const_perm_pos_arc, sc_iterator3_value(it2, 1));
      while (sc_iterator3_next(it3) == SC_TRUE)
      {
        sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it3, 0), &el_type);
        if (!(el_type & (sc_type_node_no_role | sc_type_node_role)))
          continue;

        if (sys_off == SC_TRUE
            && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it3, 1))
                || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it3, 0))))
          continue;

        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it3, 0));
        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it3, 1));

        // search of key sc-elements order
        if (SC_ADDR_IS_EQUAL(sc_iterator3_value(it3, 0), keynode_rrel_key_sc_element))
        {
          it_order2 = sc_iterator5_f_a_a_a_f_new(
              s_default_ctx,
              sc_iterator3_value(it2, 1),
              sc_type_const_common_arc,
              sc_type_const_perm_pos_arc,
              sc_type_const_perm_pos_arc,
              keynode_nrel_key_sc_element_base_order);
          while (sc_iterator5_next(it_order2) == SC_TRUE)
          {
            if (sys_off == SC_TRUE
                && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it_order2, 1))
                    || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it_order2, 3))))
              continue;

            appendIntoResult(s_default_ctx, result, sc_iterator5_value(it_order2, 1));
            appendIntoResult(s_default_ctx, result, sc_iterator5_value(it_order2, 3));
            if (SC_FALSE == key_order_found)
            {
              key_order_found = SC_TRUE;
              appendIntoResult(s_default_ctx, result, keynode_nrel_key_sc_element_base_order);
            }
          }
          sc_iterator5_free(it_order2);
        }
      }
      sc_iterator3_free(it3);

      // check if element is an sc-link
      if (SC_RESULT_OK == sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it2, 2), &el_type)
          && (el_type | sc_type_node_link))
      {
        // iterate incoming sc-arcs for link
        it3 = sc_iterator3_a_a_f_new(
            s_default_ctx, sc_type_node | sc_type_const, sc_type_const_perm_pos_arc, sc_iterator3_value(it2, 2));
        while (sc_iterator3_next(it3) == SC_TRUE)
        {
          if (sc_helper_check_arc(
                  s_default_ctx, keynode_languages, sc_iterator3_value(it3, 0), sc_type_const_perm_pos_arc)
              == SC_TRUE)
          {
            if (sys_off == SC_TRUE
                && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it3, 1))
                    || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it3, 0))))
              continue;

            appendIntoResult(s_default_ctx, result, sc_iterator3_value(it3, 0));
            appendIntoResult(s_default_ctx, result, sc_iterator3_value(it3, 1));

            search_arc_components(sc_iterator3_value(it3, 0), result, sys_off);
          }
        }
        sc_iterator3_free(it3);
      }
    }
    sc_iterator3_free(it2);
  }
  sc_iterator3_free(it1);

  connect_result_to_action(s_default_ctx, action, result);
  finish_action(s_default_ctx, action);

  return SC_RESULT_OK;
}

sc_result agent_search_links_of_relation_connected_with_element(sc_event_subscription const * event, sc_addr arg)
{
  sc_addr action, result, param_elem, param_rel;
  sc_iterator3 *it1, *it2, *it3, *it4;
  sc_iterator5 *it5, *it_order;
  sc_type el_type;
  sc_bool sys_off = SC_TRUE;
  sc_bool param_elem_found = SC_FALSE, param_rel_found = SC_FALSE, found = SC_FALSE;

  if (!sc_memory_get_arc_end(s_default_ctx, arg, &action))
    return SC_RESULT_ERROR_INVALID_PARAMS;

  // check action type
  if (sc_helper_check_arc(
          s_default_ctx,
          keynode_action_search_links_of_relation_connected_with_element,
          action,
          sc_type_const_perm_pos_arc)
      == SC_FALSE)
    return SC_RESULT_ERROR_INVALID_TYPE;

  result = create_result_node(s_default_ctx);

  // get action arguments
  it5 = sc_iterator5_f_a_a_a_a_new(
      s_default_ctx, action, sc_type_const_perm_pos_arc, 0, sc_type_const_perm_pos_arc, sc_type_node | sc_type_const);
  while (sc_iterator5_next(it5) == SC_TRUE)
  {
    if (SC_ADDR_IS_EQUAL(sc_iterator5_value(it5, 4), keynode_rrel_1))
    {
      param_elem = sc_iterator5_value(it5, 2);
      param_elem_found = SC_TRUE;
      continue;
    }
    if (SC_ADDR_IS_EQUAL(sc_iterator5_value(it5, 4), keynode_rrel_2))
    {
      param_rel = sc_iterator5_value(it5, 2);
      param_rel_found = SC_TRUE;
      continue;
    }
  }
  sc_iterator5_free(it5);
  if (param_elem_found == SC_FALSE || param_rel_found == SC_FALSE)
  {
    return SC_RESULT_ERROR;
  }

  appendIntoResult(s_default_ctx, result, param_elem);

  if (IS_SYSTEM_ELEMENT(s_default_ctx, param_elem) || IS_SYSTEM_ELEMENT(s_default_ctx, param_rel))
    sys_off = SC_FALSE;

  search_translation(param_elem, result, sys_off);

  if (SC_TRUE
      == sc_helper_check_arc(s_default_ctx, keynode_quasybinary_relation, param_rel, sc_type_const_perm_pos_arc))
  {
    // Search subclasses in quasybinary relation
    // Iterate incoming sc-arcs of quasybinary relation
    it5 = sc_iterator5_a_a_f_a_f_new(
        s_default_ctx,
        sc_type_const,
        sc_type_const | sc_type_common_arc,
        param_elem,
        sc_type_const_perm_pos_arc,
        param_rel);
    while (sc_iterator5_next(it5) == SC_TRUE)
    {
      if (sys_off == SC_TRUE
          && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 0))
              || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 1))
              || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 3))))
        continue;

      found = SC_TRUE;

      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 0));
      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 1));
      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 3));

      search_translation(sc_iterator5_value(it5, 0), result, sys_off);

      search_arc_components(sc_iterator5_value(it5, 0), result, sys_off);

      // Iterate subclasses in quasybinary relation
      it1 = sc_iterator3_f_a_a_new(s_default_ctx, sc_iterator5_value(it5, 0), sc_type_const_perm_pos_arc, 0);
      while (sc_iterator3_next(it1) == SC_TRUE)
      {
        if (sys_off == SC_TRUE
            && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it1, 1))
                || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it1, 2))))
          continue;

        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it1, 1));
        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it1, 2));

        search_translation(sc_iterator3_value(it1, 2), result, sys_off);

        search_arc_components(sc_iterator3_value(it1, 2), result, sys_off);

        // iterate order relations between elements
        it_order = sc_iterator5_f_a_a_a_a_new(
            s_default_ctx,
            sc_iterator3_value(it1, 2),
            sc_type_const_common_arc,
            sc_type_node | sc_type_const,
            sc_type_const_perm_pos_arc,
            sc_type_node | sc_type_const);
        while (sc_iterator5_next(it_order) == SC_TRUE)
        {
          if (sys_off == SC_TRUE
              && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it_order, 1))
                  || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it_order, 2))
                  || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it_order, 3))
                  || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it_order, 4))))
            continue;

          if (SC_FALSE
              == sc_helper_check_arc(
                  s_default_ctx, keynode_order_relation, sc_iterator5_value(it_order, 4), sc_type_const_perm_pos_arc))
            continue;
          if (SC_FALSE
              == sc_helper_check_arc(
                  s_default_ctx,
                  sc_iterator5_value(it5, 0),
                  sc_iterator5_value(it_order, 2),
                  sc_type_const_perm_pos_arc))
            continue;

          appendIntoResult(s_default_ctx, result, sc_iterator5_value(it_order, 1));
          appendIntoResult(s_default_ctx, result, sc_iterator5_value(it_order, 2));
          appendIntoResult(s_default_ctx, result, sc_iterator5_value(it_order, 3));
          appendIntoResult(s_default_ctx, result, sc_iterator5_value(it_order, 4));
        }
        sc_iterator5_free(it_order);

        // iterate roles of element in link
        it2 = sc_iterator3_a_a_f_new(
            s_default_ctx, sc_type_node | sc_type_const, sc_type_const_perm_pos_arc, sc_iterator3_value(it1, 1));
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
          sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it2, 0), &el_type);
          if (!(el_type & sc_type_node_role))
            continue;

          if (sys_off == SC_TRUE
              && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it2, 0))
                  || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it2, 1))))
            continue;

          appendIntoResult(s_default_ctx, result, sc_iterator3_value(it2, 0));
          appendIntoResult(s_default_ctx, result, sc_iterator3_value(it2, 1));
        }
        sc_iterator3_free(it2);
      }
      sc_iterator3_free(it1);
    }
    sc_iterator5_free(it5);

    // Iterate incoming sc-arcs of quasybinary relation
    it1 = sc_iterator3_a_a_f_new(s_default_ctx, sc_type_const | sc_type_node, sc_type_const_perm_pos_arc, param_elem);
    while (sc_iterator3_next(it1) == SC_TRUE)
    {
      if (sys_off == SC_TRUE
          && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it1, 0))
              || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it1, 1))))
        continue;

      // search all parents in quasybinary relation
      it5 = sc_iterator5_f_a_a_a_f_new(
          s_default_ctx,
          sc_iterator3_value(it1, 0),
          sc_type_const_common_arc,
          sc_type_node | sc_type_const,
          sc_type_const_perm_pos_arc,
          param_rel);
      if (sc_iterator5_next(it5) == SC_TRUE)
      {
        if (sys_off == SC_TRUE
            && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 1))
                || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 2))
                || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 3))))
          continue;

        found = SC_TRUE;

        appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 1));
        appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 2));
        appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 3));

        search_translation(sc_iterator5_value(it5, 2), result, sys_off);
        search_arc_components(sc_iterator5_value(it5, 2), result, sys_off);

        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it1, 0));
        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it1, 1));

        search_arc_components(sc_iterator3_value(it1, 0), result, sys_off);
      }
      sc_iterator5_free(it5);
    }
    sc_iterator3_free(it1);
  }
  else
  {
    // Iterate outgoing sc-arcs of given relation
    it5 = sc_iterator5_f_a_a_a_f_new(
        s_default_ctx, param_elem, sc_type_const, sc_type_const, sc_type_const_perm_pos_arc, param_rel);
    while (sc_iterator5_next(it5) == SC_TRUE)
    {
      if (sys_off == SC_TRUE
          && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 1))
              || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 2))
              || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 3))))
        continue;

      found = SC_TRUE;

      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 1));
      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 2));
      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 3));

      search_translation(sc_iterator5_value(it5, 2), result, sys_off);
      search_arc_components(sc_iterator5_value(it5, 2), result, sys_off);
    }
    sc_iterator5_free(it5);

    // Iterate incoming sc-arcs of given relation
    it5 = sc_iterator5_a_a_f_a_f_new(
        s_default_ctx, sc_type_const, sc_type_const, param_elem, sc_type_const_perm_pos_arc, param_rel);
    while (sc_iterator5_next(it5) == SC_TRUE)
    {
      if (sys_off == SC_TRUE
          && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 0))
              || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 1))
              || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator5_value(it5, 3))))
        continue;

      found = SC_TRUE;

      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 0));
      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 1));
      appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 3));

      search_translation(sc_iterator5_value(it5, 0), result, sys_off);
      search_arc_components(sc_iterator5_value(it5, 0), result, sys_off);
    }
    sc_iterator5_free(it5);

    // Iterate incoming sc-arcs for input element
    it1 = sc_iterator3_a_a_f_new(s_default_ctx, sc_type_const, sc_type_const_perm_pos_arc, param_elem);
    while (sc_iterator3_next(it1) == SC_TRUE)
    {
      if (sys_off == SC_TRUE
          && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it1, 0))
              || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it1, 1))))
        continue;

      // Iterate incoming sc-arcs for input element
      it2 = sc_iterator3_f_a_f_new(s_default_ctx, param_rel, sc_type_const_perm_pos_arc, sc_iterator3_value(it1, 0));
      if (sc_iterator3_next(it2) == SC_TRUE)
      {
        if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it2, 1))))
          continue;

        found = SC_TRUE;

        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it2, 1));
        appendIntoResult(s_default_ctx, result, sc_iterator3_value(it1, 0));

        // Iterate elements of found link of given relation
        it3 = sc_iterator3_f_a_a_new(
            s_default_ctx, sc_iterator3_value(it1, 0), sc_type_const_perm_pos_arc, sc_type_const);
        while (sc_iterator3_next(it3) == SC_TRUE)
        {
          if (sys_off == SC_TRUE
              && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it3, 1))
                  || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it3, 2))))
            continue;

          appendIntoResult(s_default_ctx, result, sc_iterator3_value(it3, 1));
          appendIntoResult(s_default_ctx, result, sc_iterator3_value(it3, 2));

          search_translation(sc_iterator3_value(it3, 2), result, sys_off);
          search_arc_components(sc_iterator3_value(it3, 2), result, sys_off);

          // Iterate role relations
          it4 = sc_iterator3_a_a_f_new(
              s_default_ctx, sc_type_const | sc_type_node, sc_type_const_perm_pos_arc, sc_iterator3_value(it3, 1));
          while (sc_iterator3_next(it4) == SC_TRUE)
          {
            if (sys_off == SC_TRUE
                && (IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it4, 0))
                    || IS_SYSTEM_ELEMENT(s_default_ctx, sc_iterator3_value(it4, 1))))
              continue;

            appendIntoResult(s_default_ctx, result, sc_iterator3_value(it4, 0));
            appendIntoResult(s_default_ctx, result, sc_iterator3_value(it4, 1));

            search_arc_components(sc_iterator3_value(it4, 0), result, sys_off);
          }
          sc_iterator3_free(it4);
        }
        sc_iterator3_free(it3);
      }
      sc_iterator3_free(it2);
    }
    sc_iterator3_free(it1);
  }

  if (found == SC_TRUE)
  {
    appendIntoResult(s_default_ctx, result, param_rel);
  }

  connect_result_to_action(s_default_ctx, action, result);
  finish_action(s_default_ctx, action);

  return SC_RESULT_OK;
}
