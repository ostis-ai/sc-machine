/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_dictionary_h_
#define _sc_dictionary_h_

#include "../../sc_types.h"

#define sc_dc_node_access_lvl_add_mask(node_mask, mask) ((node_mask) |= (mask))
#define sc_dc_node_access_lvl_remove_mask(node_mask, mask) ((node_mask) &= ~(mask))
#define sc_dc_node_access_lvl_check_mask(node_mask, mask) (((node_mask) & (mask)) == (mask))

//! A sc-dictionary structure node to store prefixes
typedef struct _sc_dictionary_node
{
  struct _sc_dictionary_node *** next;  // a pointer to sc-dictionary node children pointers
  sc_char * offset;                     // a pointer to substring of node string
  sc_uint32 offset_size;                // size to substring of node string
  void * data;                          // storing data
  sc_uint8 mask;                        // mask for rights checking and memory optimization
} sc_dictionary_node;

//! A sc-dictionary structure node to store pairs of <string, object> type
typedef struct _sc_dictionary
{
  sc_dictionary_node * root;  // sc-dictionary tree root node
  sc_uint8 size;              // default sc-dictionary node children size
  void (*char_to_int)(sc_char, sc_uint8 *, const sc_uint8 *);
} sc_dictionary;

/*! Initializes sc-dictionary
 * @param[out] dictionary Pointer to a sc-dictionary pointer to initialize
 * @param[in] children_size SC-dictionary node children count
 * @param[in] char_to_int Pointer to function that converts sc_char to sc_uint8 and returns sc_uint8 mask
 * @returns Returns SC_TRUE, if sc-dictionary didn't exist; otherwise return SC_FALSE.
 */
sc_bool sc_dictionary_initialize(
    sc_dictionary ** dictionary,
    sc_uint8 children_size,
    void (*char_to_int)(sc_char, sc_uint8 *, const sc_uint8 *));

/*! Destroys a sc-dictionary
 * @param dictionary A sc-dictionary pointer to destroy
 * @param node_destroy A painter to sc-dictionary node destroy method that passes that node to clear it
 * @returns Returns SC_TRUE, if a sc-dictionary exists; otherwise return SC_FALSE.
 */
sc_bool sc_dictionary_destroy(sc_dictionary * dictionary, void (*node_clear)(sc_dictionary_node *));

/*! Appends a string to a sc-dictionary by a common prefix with another string started in sc-dictionary node, if such
 * exists. In end sc-dictionary node stores pointer to data by string.
 * @param dictionary A sc-dictionary pointer where common prefix may be started
 * @param string An appendable string
 * @param string_size An appendable string size
 * @param data A pointer to data storing by appended string
 * @returns Returns A sc-dictionary node where appended string ends
 */
sc_dictionary_node * sc_dictionary_append(
    sc_dictionary * dictionary,
    const sc_char * string,
    sc_uint32 string_size,
    void * data);

/*! Checks, if a string is in a sc-dictionary by a common prefix with another string started in sc-dictionary
 * node, if such exists.
 * @param dictionary A sc-dictionary pointer
 * @param string A verifiable string
 * @param string_size A verifiable string size
 * @returns Returns SC_TRUE, if string starts in sc-dictionary node; otherwise return SC_FALSE.
 */
sc_bool sc_dictionary_has(const sc_dictionary * dictionary, const sc_char * string, sc_uint32 string_size);

/*! Gets data from a terminal sc-dictionary node where string ends.
 * @param dictionary A sc-dictionary pointer
 * @param string A string to retrieve data by it
 * @param string_size A string size
 * @returns Returns Data from a sc-dictionary node where string ends
 */
void * sc_dictionary_get_by_key(const sc_dictionary * dictionary, const sc_char * string, sc_uint32 string_size);

/*! Visit data in sc-dictionary nodes where key prefix ends.
 * @param dictionary A sc-dictionary pointer
 * @param string A string to retrieve data by it
 * @param string_size A string size
 * @param callable A callback to form result
 * @param dest A callback params
 * @returns Returns Data from a sc-dictionary node where string ends
 */
sc_bool sc_dictionary_get_by_key_prefix(
    const sc_dictionary * dictionary,
    const sc_char * string,
    sc_uint32 string_size,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest);

/*! Visits all sc-dictionary nodes and calls procedure with it and its data. A method completes down iterating visiting.
 * @param dictionary A sc-dictionary pointer
 * @param callable A callable object (procedure)
 * @param[out] dest A pointer to procedure result pointer
 */
sc_bool sc_dictionary_visit_down_nodes(
    sc_dictionary const * dictionary,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest);

/*! Visits all sc-dictionary nodes and calls procedure with it and its data. A method completes up iterating visiting.
 * @param node A sc-dictionary
 * @param callable A callable object (procedure)
 * @param[out] dest A pointer to procedure result pointer
 */
sc_bool sc_dictionary_visit_up_nodes(
    sc_dictionary const * dictionary,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest);

#endif
