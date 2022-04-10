/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_dictionary_h_
#define _sc_dictionary_h_

#include "../../sc_types.h"
#include "../sc-list/sc_list.h"

typedef struct _sc_dictionary_node
{
  struct _sc_dictionary_node ** next;
  sc_char * offset;
  sc_uint16 offset_size;
  sc_list * data_list;
  sc_uint8 mask;
} sc_dictionary_node;

typedef struct _sc_dictionary
{
  sc_dictionary_node * root;
  sc_uint8 size;
} sc_dictionary;

typedef struct _sc_link_content
{
  sc_dictionary_node * node;
  sc_char * sc_string;
  sc_uint32 string_size;
} sc_link_content;

sc_bool sc_dictionary_initialize(sc_dictionary ** dictionary);

sc_bool sc_dictionary_shutdown(sc_dictionary * dictionary);

sc_dictionary_node * sc_dictionary_append_to_node(sc_dictionary_node * node, sc_char * sc_string, sc_uint32 size);

sc_dictionary_node * sc_dictionary_append(
    sc_dictionary * dictionary,
    sc_char * sc_string,
    sc_uint32 size,
    void * value);

sc_dictionary_node * sc_dictionary_remove_from_node(
    sc_dictionary_node * node,
    const sc_char * sc_string,
    sc_uint32 index);

sc_bool sc_dictionary_remove(sc_dictionary * dictionary, const sc_char * sc_string);

sc_bool sc_dictionary_is_in_from_node(sc_dictionary_node * node, const sc_char * sc_string);

sc_bool sc_dictionary_is_in(sc_dictionary * dictionary, const sc_char * sc_string);

sc_dictionary_node * sc_dictionary_get_last_node_from_node(sc_dictionary_node * node, const sc_char * sc_string);

void * sc_dictionary_get_data_from_node(sc_dictionary_node * node, const sc_char * sc_string);

sc_list * sc_dictionary_get_datas_from_node(sc_dictionary_node * node, const sc_char * sc_string);

sc_char * sc_dictionary_get_sc_string_from_node(sc_dictionary_node * node, sc_addr addr, sc_uint32 * count);

void sc_dictionary_show_from_node(sc_dictionary_node * node, sc_char * tab);

void sc_dictionary_show(sc_dictionary * dictionary);

void sc_dictionary_visit_node_from_node(
    sc_dictionary_node * node,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest);

void sc_dictionary_visit_nodes(
    sc_dictionary * dictionary,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest);

sc_uint32 sc_addr_to_hash(sc_addr addr);

sc_char * sc_addr_to_str(sc_addr addr);

#endif
