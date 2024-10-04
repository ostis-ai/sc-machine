/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_dictionary_private_h_
#define _sc_dictionary_private_h_

#include "sc-core/sc_types.h"
#include "sc-core/sc-container/sc_list.h"

sc_dictionary_node * _sc_dictionary_node_initialize(sc_uint8 children_size);

sc_dictionary_node * _sc_dictionary_get_next_node(
    sc_dictionary const * dictionary,
    sc_dictionary_node const * node,
    sc_char ch);

/*! Appends a string to a sc-dictionary by a common prefix with another string started in sc-dictionary node, if such
 * exists.
 * @param dictionary A sc-dictionary pointer
 * @param string An appendable string
 * @param string_size An appendable string size
 * @returns Returns A sc-dictionary node where appended string is ended
 */
sc_dictionary_node * sc_dictionary_append_to_node(
    sc_dictionary * dictionary,
    sc_char const * string,
    sc_uint32 string_size);

/*! Gets a terminal sc-dictionary node where string ends.
 * @param dictionary A sc-dictionary pointer
 * @param node A sc-dictionary node where common prefix may be started
 * @param string A string to retrieve data by it
 * @param string_size A string size
 * @returns Returns A sc-dictionary node where string ends
 */
sc_dictionary_node const * sc_dictionary_get_last_node_from_node(
    sc_dictionary * dictionary,
    sc_dictionary_node const * node,
    sc_char const * string,
    sc_uint32 string_size);

/*! Visits all sc-dictionary nodes starting with specified node and calls procedure with it and its data. A method
 * completes down iterating visiting.
 * @param dictionary A sc-dictionary pointer
 * @param node A sc-dictionary node to start visiting
 * @param callable A callable object (procedure)
 * @param[out] dest A pointer to procedure result pointer
 */
sc_bool sc_dictionary_visit_down_node_from_node(
    sc_dictionary const * dictionary,
    sc_dictionary_node * node,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest);

/*! Visits all sc-dictionary nodes starting with specified node and calls procedure with it and its data. A method
 * completes up iterating visiting.
 * @param dictionary A sc-dictionary pointer
 * @param node A sc-dictionary node to start visiting
 * @param callable A callable object (procedure)
 * @param[out] dest A pointer to procedure result pointer
 */
sc_bool sc_dictionary_visit_up_node_from_node(
    sc_dictionary const * dictionary,
    sc_dictionary_node * node,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest);

#endif
