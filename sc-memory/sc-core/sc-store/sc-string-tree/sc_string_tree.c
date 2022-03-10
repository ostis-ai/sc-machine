/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_string_tree.h"
#include "stdio.h"

#include <glib.h>

static const sc_uint16 s_min_sc_char = 1;
static const sc_uint16 s_max_sc_char = 255;
static const sc_uint64 s_max_addr = (sc_uint64)2 << 16;

sc_string_tree *tree;
sc_string_tree_addr_table *table;

#define SC_STRING_TREE_NODE_IS_VALID(__node) ((__node) != null_ptr)

sc_bool sc_string_tree_initialize()
{
  if (tree != null_ptr)
    return SC_FALSE;

  if (table != null_ptr)
    return SC_FALSE;

  tree = g_new0(sc_string_tree, 1);
  tree->root = _sc_string_tree_node_initialize();

  table = g_new0(sc_string_tree_addr_table, 1);
  table->nodes = g_new0(sc_string_tree_node*, s_max_addr);

  return SC_TRUE;
}

sc_bool sc_string_tree_shutdown()
{
  if (tree == null_ptr)
    return SC_FALSE;

  tree = null_ptr;

  g_free(table);
  table = null_ptr;

  return SC_TRUE;
}

inline sc_uint16 sc_string_tree_children_size()
{
  return s_max_sc_char - s_min_sc_char + 2;
}

inline sc_string_tree_node* _sc_string_tree_node_initialize()
{
  sc_string_tree_node *node = g_new0(sc_string_tree_node, 1);

  node->next = g_new0(sc_string_tree_node*, sc_string_tree_children_size());

  sc_uint16 i;
  for (i = 0; i < sc_string_tree_children_size(); ++i)
    node->next[i] = null_ptr;

  node->is_terminal = SC_FALSE;
  node->addrs = null_ptr;
  node->addrs_size = 0;
  node->string_size = 0;

  return node;
}

inline sc_string_tree_node* _sc_string_tree_get_next_node(sc_string_tree_node *node, sc_char ch)
{
  sc_uint16 num = sc_char_to_sc_int(ch);
  return node->next[num];
}

inline sc_string_tree_node* _sc_string_tree_get_parent(sc_string_tree_node *node)
{
  return node->next[0];
}

sc_string_tree_node* sc_string_tree_append_to_node(sc_string_tree_node *node, sc_addr addr, const sc_char *sc_string, sc_uint16 size)
{
  sc_uint16 i;
  sc_string_tree_node *parent = null_ptr;
  sc_uint16 parent_id = 0;
  for (i = 0; i < size; ++i)
  {
    sc_uint16 num = sc_char_to_sc_int(sc_string[i]);
    if (node->next[num] == null_ptr)
    {
      node->next[num] = _sc_string_tree_node_initialize();

      node->next[0] = parent;
      node->parent_id = parent_id;
    }
    parent = node;
    parent_id = num;

    node = node->next[num];
  }
  node->is_terminal = SC_TRUE;
  node->next[0] = parent;
  node->parent_id = parent_id;
  node->string_size = size;

  if (node->addrs == null_ptr)
    node->addrs = g_new0(sc_addr, ++node->addrs_size);
  else
    node->addrs = realloc(node->addrs, ++node->addrs_size);
  node->addrs[node->addrs_size - 1] = addr;

  table->nodes[sc_addr_to_hash(addr)] = node;

  return node;
}

sc_string_tree_node* sc_string_tree_append(sc_addr addr, const sc_char *sc_string, sc_uint16 size)
{
  sc_char *old_string = sc_string_tree_get_sc_string(addr);
  if (old_string != null_ptr)
  {
    sc_string_tree_node * node = table->nodes[sc_addr_to_hash(addr)];
    sc_addr *addrs = node->addrs;
    sc_uint16 i;

    for (i = 0; i < node->addrs_size; ++i)
    {
      if (SC_ADDR_IS_EQUAL(addrs[i], addr))
        SC_ADDR_MAKE_EMPTY(addrs[i])
    }
  }

  return sc_string_tree_append_to_node(tree->root, addr, sc_string, size);
}

sc_string_tree_node * sc_string_tree_remove_from_node(sc_string_tree_node *node, const sc_char *sc_string, sc_uint16 index)
{
  sc_uint16 string_size = strlen(sc_string);
  sc_uint16 num = sc_char_to_sc_int(sc_string[index]);
  if (SC_STRING_TREE_NODE_IS_VALID(node->next[num]))
  {
    node->next[num] = sc_string_tree_remove_from_node(node->next[num], sc_string, index + 1);

    if (SC_STRING_TREE_NODE_IS_VALID(node->next[num]))
    {
      return node;
    }
  }

  if (node->is_terminal && index == string_size)
  {
    g_free(node);
    return null_ptr;
  }

  return node;
}

sc_bool sc_string_tree_remove(const sc_char *sc_string)
{
  return sc_string_tree_remove_from_node(tree->root, sc_string, 0) != null_ptr;
}

sc_bool sc_string_tree_is_in_from_node(sc_string_tree_node *node, const sc_char *sc_string)
{
  sc_uint16 i;
  sc_uint16 string_size = strlen(sc_string);
  for (i = 0; i < string_size; ++i)
  {
    sc_string_tree_node * next = _sc_string_tree_get_next_node(node, sc_string[i]);
    if (SC_STRING_TREE_NODE_IS_VALID(next))
      node = next;
    else
      break;
  }

  if (i == string_size && node->addrs_size != 0)
    return SC_TRUE;
  else
    return SC_FALSE;
}

sc_bool sc_string_tree_is_in(const sc_char *sc_string)
{
  return sc_string_tree_is_in_from_node(tree->root, sc_string);
}

sc_addr sc_string_tree_get_sc_link_from_node(sc_string_tree_node *node, const sc_char *sc_string)
{
  sc_uint16 i;
  sc_uint16 string_size = strlen(sc_string);
  for (i = 0; i < string_size; ++i)
  {
    sc_string_tree_node * next = _sc_string_tree_get_next_node(node, sc_string[i]);
    if (SC_STRING_TREE_NODE_IS_VALID(next))
      node = next;
    else
      break;
  }

  if (i == string_size && node->addrs_size != 0)
    return node->addrs[0];
  else
  {
    sc_addr empty;
    SC_ADDR_MAKE_EMPTY(empty)
    return empty;
  }
}

sc_addr sc_string_tree_get_sc_link(const sc_char *sc_string)
{
  return sc_string_tree_get_sc_link_from_node(tree->root, sc_string);
}

sc_bool sc_string_tree_get_sc_links_from_node(sc_string_tree_node *node, const sc_char *sc_string, sc_addr **addrs, sc_uint16 *size)
{
  sc_uint16 i;
  sc_uint16 string_size = strlen(sc_string);
  for (i = 0; i < string_size; ++i)
  {
    sc_string_tree_node * next = _sc_string_tree_get_next_node(node, sc_string[i]);
    if (SC_STRING_TREE_NODE_IS_VALID(next))
      node = next;
    else
      break;
  }

  if (i == string_size && node->addrs_size != 0)
  {
    *addrs = node->addrs;
    *size = node->addrs_size;
    return SC_TRUE;
  }
  else
  {
    *addrs = null_ptr;
    *size = 0;
    return SC_FALSE;
  }
}

sc_bool sc_string_tree_get_sc_links(const sc_char *sc_string, sc_addr **addrs, sc_uint16 *size)
{
  return sc_string_tree_get_sc_links_from_node(tree->root, sc_string, addrs, size);
}

sc_char* sc_string_tree_get_sc_string_from_node(sc_string_tree_node *node, sc_addr addr, sc_uint16 size, sc_uint16 *count)
{
  g_assert(size != 0);

  sc_char *sc_string = null_ptr;
  if (node->next[0] != null_ptr && node->next[0]->parent_id != 0)
    sc_string = sc_string_tree_get_sc_string_from_node(node->next[0], addr, size, count);

  if (sc_string == null_ptr)
    sc_string = g_new0(sc_char, size);

  sc_string[*count] = sc_int_to_sc_char(node->parent_id);
  ++(*count);
  return sc_string;
}

sc_char* sc_string_tree_get_sc_string(sc_addr addr)
{
  sc_string_tree_node * node = table->nodes[sc_addr_to_hash(addr)];

  if (node == null_ptr)
    return null_ptr;

  sc_uint16 count = 0;
  return sc_string_tree_get_sc_string_from_node(node, addr, node->string_size, &count);
}

void sc_string_tree_get_sc_string_ext(sc_addr addr, sc_char **sc_string, sc_uint16 *size)
{
  sc_string_tree_node * node = table->nodes[sc_addr_to_hash(addr)];
  sc_uint16 count = 0;

  *sc_string = sc_string_tree_get_sc_string_from_node(node, addr, node->string_size, &count);
  *size = node->string_size;
}

void sc_string_tree_show_from_node(sc_string_tree_node *node, sc_char *tab)
{
  sc_uint16 i;
  for (i = 1; i < sc_string_tree_children_size(); ++i)
  {
    if (node->next[i])
    {
      if (node->addrs_size != 0)
        printf("%s%c[%d]\n", tab, sc_int_to_sc_char(i), node->addrs_size);
      else
        printf("%s%c\n", tab, sc_int_to_sc_char(i));

      sc_char * new_tab = g_new0(sc_char, strlen(tab) + 6);
      strcpy(new_tab, tab);
      strcat(new_tab, "|----\0");

      sc_string_tree_show_from_node(node->next[i], new_tab);
    }
  }
}

void sc_string_tree_show()
{
  sc_string_tree_show_from_node(tree->root, "\0");
}

inline sc_uint16 sc_char_to_sc_int(sc_char ch)
{
  sc_uint16 num = (sc_uint16)ch - s_min_sc_char + 2;
  if (num < s_min_sc_char || num > s_max_sc_char)
    return 1;

  return num;
}

inline sc_char sc_int_to_sc_char(sc_uint16 num)
{
  return (sc_char)(num + s_min_sc_char - 2);
}

inline sc_uint64 sc_addr_to_hash(sc_addr addr)
{
  return (addr.seg << 16) | addr.offset;
}
