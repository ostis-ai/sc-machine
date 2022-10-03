/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_dictionary_private_h_
#define _sc_dictionary_private_h_

#include "../../sc_types.h"
#include "../sc-list/sc_list.h"

sc_dictionary_node * _sc_dictionary_node_initialize(sc_uint8 children_size);

sc_dictionary_node * _sc_dictionary_get_next_node(sc_dictionary * dictionary, sc_dictionary_node * node, sc_char ch);

sc_addr * sc_list_to_addr_array(sc_list * list);

sc_addr_hash * sc_list_to_hashes_array(sc_list * list);

/*! Appends a string to a sc-dictionary by a common prefix with another string started in sc-dictionary node, if such
 * exists.
 * @param dictionary A sc-dictionary pointer
 * @param sc_string An appendable string
 * @param size An appendable string size
 * @returns Returns A sc-dictionary node where appended string is ended
 */
sc_dictionary_node * sc_dictionary_append_to_node(
    sc_dictionary * dictionary,
    const sc_char * sc_string,
    sc_uint32 size);

/*! Removes a string from a sc-dictionary by a common prefix with another string started in sc-dictionary node, if such
 * exists. A common prefix doesn't remove form sc-dictionary if it contains in another string.
 * @param dictionary A sc-dictionary pointer
 * @param node A sc-dictionary node where common prefix may be started
 * @param sc_string A removable string
 * @param size Removable string size
 * @returns Returns A sc-dictionary node where removable string or its prefix starts
 * @note If string isn't in sc-dictionary then null_pointer will be returned
 */
sc_dictionary_node * sc_dictionary_remove_from_node(
    sc_dictionary * dictionary,
    sc_dictionary_node * node,
    const sc_char * sc_string,
    sc_uint32 index);

/*! Gets a terminal sc-dictionary node where string ends.
 * @param dictionary A sc-dictionary pointer
 * @param node A sc-dictionary node where common prefix may be started
 * @param sc_string A string to retrieve data by it
 * @returns Returns A sc-dictionary node where string ends
 */
sc_dictionary_node * sc_dictionary_get_last_node_from_node(
    sc_dictionary * dictionary,
    sc_dictionary_node * node,
    const sc_char * sc_string);

/*! Visits all sc-dictionary nodes starting with specified node and calls procedure with it and its data. A method
 * completes down iterating visiting.
 * @param dictionary A sc-dictionary pointer
 * @param node A sc-dictionary node to start visiting
 * @param callable A callable object (procedure)
 * @param[out] dest A pointer to procedure result pointer
 */
void sc_dictionary_visit_down_node_from_node(
    sc_dictionary * dictionary,
    sc_dictionary_node * node,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest);

/*! Visits all sc-dictionary nodes starting with specified node and calls procedure with it and its data. A method
 * completes up iterating visiting.
 * @param dictionary A sc-dictionary pointer
 * @param node A sc-dictionary node to start visiting
 * @param callable A callable object (procedure)
 * @param[out] dest A pointer to procedure result pointer
 */
void sc_dictionary_visit_up_node_from_node(
    sc_dictionary * dictionary,
    sc_dictionary_node * node,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest);

#endif
