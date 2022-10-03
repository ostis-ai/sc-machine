/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_dictionary_h_
#define _sc_dictionary_h_

#include "../../sc_types.h"
#include "../sc-list/sc_list.h"

#define SC_DC_NODE_ACCESS_LVL_RMASK 0b00000001
#define SC_DC_NODE_ACCESS_LVL_WMASK 0b00000010

#define sc_dc_node_access_lvl_make_read(node) (node->mask |= SC_DC_NODE_ACCESS_LVL_RMASK)
#define sc_dc_node_access_lvl_make_no_read(node) (node->mask &= ~SC_DC_NODE_ACCESS_LVL_RMASK)
#define sc_dc_node_access_lvl_check_read(node) \
  ((node->mask & SC_DC_NODE_ACCESS_LVL_RMASK) == SC_DC_NODE_ACCESS_LVL_RMASK)

#define sc_dc_node_access_lvl_make_write(node) (node->mask |= SC_DC_NODE_ACCESS_LVL_WMASK)
#define sc_dc_node_access_lvl_make_no_write(node) (node->mask &= ~SC_DC_NODE_ACCESS_LVL_WMASK)
#define sc_dc_node_access_lvl_check_write(node) \
  ((node->mask & SC_DC_NODE_ACCESS_LVL_WMASK) == SC_DC_NODE_ACCESS_LVL_WMASK)

//! A sc-dictionary structure node to store prefixes
typedef struct _sc_dictionary_node
{
  struct _sc_dictionary_node ** next;  // a pointer to sc-dictionary node children pointers
  sc_char * offset;                    // a pointer to substring of node string
  sc_uint32 offset_size;               // size to substring of node string
  sc_list * data_list;                 // data list
  sc_uint8 mask;                       // mask for rights checking and memory optimization
} sc_dictionary_node;

//! A sc-dictionary structure node to store pairs of <string, object> type
typedef struct _sc_dictionary
{
  sc_dictionary_node * root;  // sc-dictionary tree root node
  sc_uint8 size;              // default sc-dictionary node children size
  void (*char_to_int)(sc_char, sc_uint8 *, sc_uint8 *);
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
    void (*char_to_int)(sc_char, sc_uint8 *, sc_uint8 *));

/*! Destroys a sc-dictionary
 * @param dictionary A sc-dictionary pointer to destroy
 * @param node_destroy A painter to sc-dictionary node destroy method that passes that node and additional args
 * @returns Returns SC_TRUE, if a sc-dictionary exists; otherwise return SC_FALSE.
 */
sc_bool sc_dictionary_destroy(sc_dictionary * dictionary, void (*node_destroy)(sc_dictionary_node *, void **));

/*! Destroys a sc-dictionary node
 * @param node A sc-dictionary node pointer to destroy
 * @param args An additional args used with node destroying
 */
void sc_dictionary_node_destroy(sc_dictionary_node * node, void ** args);

/*! Appends a string to a sc-dictionary by a common prefix with another string started in sc-dictionary node, if such
 * exists. In end sc-dictionary node stores pointer to data by string.
 * @param dictionary A sc-dictionary pointer where common prefix may be started
 * @param sc_string An appendable string
 * @param size An appendable string size
 * @param data A pointer to data storing by appended string
 * @returns Returns A sc-dictionary node where appended string ends
 */
sc_dictionary_node * sc_dictionary_append(
    sc_dictionary * dictionary,
    const sc_char * sc_string,
    sc_uint32 size,
    void * data);

/*! Removes a string from a sc-dictionary by a common prefix with another string started in sc-dictionary node, if such
 * exists. A common prefix doesn't remove form sc-dictionary if it contains in another string.
 * @param sc_dictionary A sc-dictionary where common prefix may be started
 * @param sc_string A removable string
 * @returns Returns If a string was removed then function returns SC_TRUE; otherwise return SC_FALSE.
 * @note If a string isn't in sc-dictionary then SC_FALSE will be returned
 */
sc_bool sc_dictionary_remove(sc_dictionary * dictionary, const sc_char * sc_string);

/*! Checks, if a string is in a sc-dictionary by a common prefix with another string started in sc-dictionary
 * node, if such exists.
 * @param dictionary A sc-dictionary pointer
 * @param sc_string A verifiable string
 * @returns Returns SC_TRUE, if string starts in sc-dictionary node; otherwise return SC_FALSE.
 */
sc_bool sc_dictionary_is_in(sc_dictionary * dictionary, const sc_char * sc_string);

/*! Gets first data from a terminal sc-dictionary node where string ends.
 * @param dictionary A sc-dictionary pointer
 * @param node A sc-dictionary node where common prefix may be started
 * @param sc_string A string to retrieve data by it
 * @returns Returns Data from a sc-dictionary node where string ends
 */
void * sc_dictionary_get_first_data_from_node(
    sc_dictionary * dictionary,
    sc_dictionary_node * node,
    const sc_char * sc_string);

/*! Gets datas from a terminal sc-dictionary node where string ends.
 * @param dictionary A sc-dictionary pointer
 * @param sc_string A string to retrieve datas by it
 * @returns Returns Datas from a sc-dictionary node where string ends
 */
sc_list * sc_dictionary_get(sc_dictionary * dictionary, const sc_char * sc_string);

/*! Visits all sc-dictionary nodes and calls procedure with it and its data. A method completes down iterating visiting.
 * @param dictionary A sc-dictionary pointer
 * @param callable A callable object (procedure)
 * @param[out] dest A pointer to procedure result pointer
 */
void sc_dictionary_visit_down_nodes(
    sc_dictionary * dictionary,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest);

/*! Visits all sc-dictionary nodes and calls procedure with it and its data. A method completes up iterating visiting.
 * @param node A sc-dictionary
 * @param callable A callable object (procedure)
 * @param[out] dest A pointer to procedure result pointer
 */
void sc_dictionary_visit_up_nodes(
    sc_dictionary * dictionary,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest);

#endif
