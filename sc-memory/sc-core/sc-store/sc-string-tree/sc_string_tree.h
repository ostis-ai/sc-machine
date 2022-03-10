/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_string_tree_h_
#define _sc_string_tree_h_

#include "../sc_types.h"

typedef struct _sc_string_tree_node
{
  struct _sc_string_tree_node **next;
  sc_bool is_terminal;
  sc_uint16 parent_id;
  sc_uint16 string_size;
  sc_addr *addrs;
  sc_uint16 addrs_size;
} sc_string_tree_node;

typedef struct _sc_string_tree
{
  sc_string_tree_node *root;
  sc_uint16 size;
} sc_string_tree;

typedef struct _sc_string_tree_addr_table
{
  sc_string_tree_node **nodes;
} sc_string_tree_addr_table;

sc_bool sc_string_tree_initialize();

sc_bool sc_string_tree_shutdown();

sc_uint16 sc_string_tree_children_size();

sc_string_tree_node* _sc_string_tree_node_initialize();

sc_string_tree_node* _sc_string_tree_get_next_node(sc_string_tree_node *node, sc_char ch);

sc_string_tree_node* sc_string_tree_append_to_node(sc_string_tree_node *node, sc_addr addr, const sc_char *sc_string, sc_uint16 size);

sc_string_tree_node* sc_string_tree_append(sc_addr addr, const sc_char *sc_string, sc_uint16 size);

sc_string_tree_node* sc_string_tree_remove_from_node(sc_string_tree_node *node, const sc_char *sc_string, sc_uint16 index);

sc_bool sc_string_tree_remove(const sc_char *sc_string);

sc_bool sc_string_tree_is_in_from_node(sc_string_tree_node *node, const sc_char *sc_string);

sc_bool sc_string_tree_is_in(const sc_char *sc_string);

sc_addr sc_string_tree_get_sc_link_from_node(sc_string_tree_node *node, const sc_char *sc_string);

sc_addr sc_string_tree_get_sc_link(const sc_char *sc_string);

sc_bool sc_string_tree_get_sc_links_from_node(sc_string_tree_node *node, const sc_char *sc_string, sc_addr **addrs, sc_uint16 *size);

sc_bool sc_string_tree_get_sc_links(const sc_char *sc_string, sc_addr **addrs, sc_uint16 *size);

sc_char* sc_string_tree_get_sc_string_from_node(sc_string_tree_node *node, sc_addr addr, sc_uint16 size, sc_uint16 *count);

sc_char* sc_string_tree_get_sc_string(sc_addr addr);

void sc_string_tree_get_sc_string_ext(sc_addr addr, sc_char **sc_string, sc_uint16 *size);

void sc_string_tree_show_from_node(sc_string_tree_node *node, sc_char *tab);

void sc_string_tree_show();

sc_uint16 sc_char_to_sc_int(sc_char ch);

sc_char sc_int_to_sc_char(sc_uint16 num);

sc_uint64 sc_addr_to_hash(sc_addr addr);

#endif
