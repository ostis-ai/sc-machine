/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_string_tree.h"
#include "stdio.h"

#include <glib.h>

static const sc_uint8 s_min_sc_char = 1;
static const sc_uint8 s_max_sc_char = 62;
static const sc_uint32 s_default_links_num = (sc_uint32)2 << 16;

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

  table->size = s_default_links_num;
  table->nodes = g_new0(sc_string_tree_node*, table->size);
  sc_uint32 i;
  for (i = 0; i < table->size; ++i)
    table->nodes[i] = null_ptr;

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

inline sc_uint8 sc_string_tree_children_size()
{
  return s_max_sc_char - s_min_sc_char + 1;
}

inline sc_string_tree_node* _sc_string_tree_node_initialize()
{
  sc_string_tree_node *node = g_new0(sc_string_tree_node, 1);

  node->next = g_new0(sc_string_tree_node*, sc_string_tree_children_size());

  sc_uint8 i;
  for (i = 0; i < sc_string_tree_children_size(); ++i)
    node->next[i] = null_ptr;

  node->hashes = null_ptr;
  node->hashes_size = 0;
  node->mask = 0x0;

  return node;
}

inline sc_string_tree_node* _sc_string_tree_get_next_node(sc_string_tree_node *node, sc_char ch)
{
  sc_uint8 num;
  sc_char_to_sc_int(ch, &num, &node->mask);
  return node->next[num];
}

inline sc_string_tree_node* _sc_string_tree_get_parent(sc_string_tree_node *node)
{
  return node->next[0];
}

sc_string_tree_node* sc_string_tree_append_to_node(sc_string_tree_node *node, sc_addr addr, const sc_char *sc_string, sc_uint32 size)
{
  sc_uint32 i;
  sc_string_tree_node *parent = null_ptr;
  sc_uint8 parent_id = 0;
  for (i = 0; i < size; ++i)
  {
    sc_uint8 num;
    sc_char_to_sc_int(sc_string[i], &num, &node->mask);
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
  node->next[0] = parent;
  node->parent_id = parent_id;

  if (node->hashes == null_ptr)
    node->hashes = g_new0(sc_addr_hash, ++node->hashes_size);
  else
    node->hashes = g_renew(sc_addr_hash, node->hashes, ++node->hashes_size);

  sc_uint32 hash = sc_addr_to_hash(addr);
  node->hashes[node->hashes_size - 1] = hash;

  if (table->size <= hash)
  {
    table->size = hash + 1;
    table->nodes = g_renew(sc_string_tree_node*, table->nodes, table->size);
  }
  table->nodes[hash] = node;

  return node;
}

sc_string_tree_node* sc_string_tree_append(sc_addr addr, const sc_char *sc_string, sc_uint32 size)
{
  sc_char *old_string = sc_string_tree_get_sc_string(addr);
  if (old_string != null_ptr)
  {
    sc_string_tree_node * node = table->nodes[sc_addr_to_hash(addr)];
    sc_addr_hash *hashes = node->hashes;

    sc_uint8 i;
    for (i = 0; i < node->hashes_size; ++i)
    {
      sc_addr other;
      other.seg = SC_ADDR_LOCAL_SEG_FROM_INT(hashes[i]);
      other.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(hashes[i]);

      if (SC_ADDR_IS_EQUAL(other, other))
        node->hashes[i] = 0;
    }
  }

  return sc_string_tree_append_to_node(tree->root, addr, sc_string, size);
}

sc_string_tree_node * sc_string_tree_remove_from_node(sc_string_tree_node *node, const sc_char *sc_string, sc_uint32 index)
{
  sc_uint32 string_size = strlen(sc_string);
  sc_uint8 num;
  sc_char_to_sc_int(sc_string[index], &num, &node->mask);
  if (SC_STRING_TREE_NODE_IS_VALID(node->next[num]))
  {
    node->next[num] = sc_string_tree_remove_from_node(node->next[num], sc_string, index + 1);

    if (SC_STRING_TREE_NODE_IS_VALID(node->next[num]))
      return node;
  }

  if (node->hashes_size != 0 && index == string_size)
    return null_ptr;

  return node;
}

sc_bool sc_string_tree_remove(const sc_char *sc_string)
{
  return sc_string_tree_remove_from_node(tree->root, sc_string, 0) != null_ptr;
}

sc_bool sc_string_tree_is_in_from_node(sc_string_tree_node *node, const sc_char *sc_string)
{
  sc_uint32 i;
  sc_uint32 string_size = strlen(sc_string);
  for (i = 0; i < string_size; ++i)
  {
    sc_string_tree_node * next = _sc_string_tree_get_next_node(node, sc_string[i]);
    if (SC_STRING_TREE_NODE_IS_VALID(next))
      node = next;
    else
      break;
  }

  if (node->hashes_size != 0 && i == string_size)
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
  sc_uint32 i;
  sc_uint32 string_size = strlen(sc_string);
  for (i = 0; i < string_size; ++i)
  {
    sc_string_tree_node * next = _sc_string_tree_get_next_node(node, sc_string[i]);
    if (SC_STRING_TREE_NODE_IS_VALID(next))
      node = next;
    else
      break;
  }

  if (node->hashes_size != 0 && i == string_size)
  {
    sc_addr addr;
    addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(node->hashes[0]);
    addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(node->hashes[0]);

    return addr;
  }
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

sc_bool sc_string_tree_get_sc_links_from_node(sc_string_tree_node *node, const sc_char *sc_string, sc_addr **addrs, sc_uint32 *size)
{
  sc_uint32 i;
  sc_uint32 string_size = strlen(sc_string);
  for (i = 0; i < string_size; ++i)
  {
    sc_string_tree_node * next = _sc_string_tree_get_next_node(node, sc_string[i]);
    if (SC_STRING_TREE_NODE_IS_VALID(next))
      node = next;
    else
      break;
  }

  if (node->hashes_size != 0 && i == string_size)
  {
    *addrs = null_ptr;
    *size = 0;

    for (i = 0; i < node->hashes_size; ++i)
    {
      if (node->hashes[i] != 0)
      {
        if (*addrs == null_ptr)
          *addrs = g_new0(sc_addr, ++(*size));
        else
          *addrs = g_renew(sc_addr, *addrs, ++(*size));
      }
    }

    return *size ? SC_TRUE : SC_FALSE;
  }
  else
  {
    *addrs = null_ptr;
    *size = 0;
    return SC_FALSE;
  }
}

sc_bool sc_string_tree_get_sc_links(const sc_char *sc_string, sc_addr **addrs, sc_uint32 *size)
{
  return sc_string_tree_get_sc_links_from_node(tree->root, sc_string, addrs, size);
}

sc_char* sc_string_tree_get_sc_string_from_node(sc_string_tree_node *node, sc_addr addr, sc_uint32 *count)
{
  sc_char *sc_string = null_ptr;
  if (node->next[0] != null_ptr && node->next[0]->parent_id != 0)
    sc_string = sc_string_tree_get_sc_string_from_node(node->next[0], addr, count);

  if (sc_string == null_ptr)
    sc_string = g_new0(sc_char, 1);

  sc_string[*count] = (sc_char)sc_int_to_sc_char(node->parent_id, node->mask);
  ++(*count);
  return sc_string;
}

sc_char* sc_string_tree_get_sc_string(sc_addr addr)
{
  sc_uint16 hash = sc_addr_to_hash(addr);
  if (table->size <= hash)
    return null_ptr;

  sc_string_tree_node *node = table->nodes[hash];

  if (node == null_ptr)
    return null_ptr;

  sc_uint32 count = 0;
  return sc_string_tree_get_sc_string_from_node(node, addr,  &count);
}

void sc_string_tree_get_sc_string_ext(sc_addr addr, sc_char **sc_string, sc_uint32 *size)
{
  sc_uint16 hash = sc_addr_to_hash(addr);
  if (table->size <= hash)
  {
    *sc_string = null_ptr;
    *size = 0;
  }

  sc_string_tree_node *node = table->nodes[hash];
  sc_uint32 count = 0;

  *sc_string = sc_string_tree_get_sc_string_from_node(node, addr, &count);
  *size = count;
}

void sc_string_tree_show_from_node(sc_string_tree_node *node, sc_char *tab)
{
  sc_uint8 i;
  for (i = 1; i < sc_string_tree_children_size(); ++i)
  {
    if (node->next[i])
    {
      if (node->hashes_size != 0)
        printf("%s%c[%d]\n", tab, sc_int_to_sc_char(i, node->mask), node->hashes_size);
      else
        printf("%s%c\n", tab, sc_int_to_sc_char(i, node->mask));

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

inline void sc_char_to_sc_int(sc_uchar ch, sc_uint8 *ch_num, sc_uint8 *mask)
{
  sc_uint8 num = 1;

  if (ch >= '0' && ch <= '9')
    num += 1 + (ch - '0'); // 2 - 11

  else if ((ch >= 'a' && ch <= 'z'))
    num += 12 + (ch - 'a'); // 12 - 37

  else if (ch >= 'A' && ch <= 'Z')
    num += 12 + (ch - 'A'); // 12 - 37

  else if (ch == '=')
    num = 38;

  else if (ch == '_')
    num = 39;

  else if (ch == '<')
    num = 40;

  else if (ch == '>')
    num = 41;

  else if (ch == '(')
    num = 42;

  else if (ch == ')')
    num = 43;

  else if (ch == '[')
    num = 44;

  else if (ch == ']')
    num = 45;

  else if (ch == '{')
    num = 46;

  else if (ch == '}')
    num = 47;

  else if (ch == '.')
    num = 48;

  else if (ch == ',')
    num = 49;

  else if (ch == ';')
    num = 50;

  else if (ch == ':')
    num = 51;

  else if (ch == '!')
    num = 52;

  else if (ch == '?')
    num = 53;

  else if (ch == '\"')
    num = 54;

  else if (ch == '\'')
    num = 55;

  else if (ch == '`')
    num = 56;

  else if (ch == '-')
    num = 57;

  else if (ch == '+')
    num = 58;

  else if (ch == '\\')
    num = 59;

  else if (ch == '/')
    num = 60;

  else if (ch == '*')
    num = 61;

  else if (ch == '#')
    num = 62;

  *ch_num = num;
}

inline sc_uchar sc_int_to_sc_char(sc_uint8 num, sc_uint8 mask)
{
  sc_uchar ch = '@';

  if (num >= 2 && num <= 11)
    ch = (sc_char)('0' + (num - 2)); // 2 - 11

  else if (num >= 12 && num <= 38)
    ch = (sc_char)('a' + (num - 13)); // 12 - 37

  else if (num == 38)
    ch = '=';

  else if (num == 39)
    ch = '_';

  else if (num == 40)
    ch = '<';

  else if (num == 41)
    ch = '>';

  else if (num == 42)
    ch = '(';

  else if (num == 43)
    ch = ')';

  else if (num == 44)
    ch = '[';

  else if (num == 45)
    ch = ']';

  else if (num == 46)
    ch = '{';

  else if (num == 47)
    ch = '}';

  else if (num == 48)
    ch = '.';

  else if (num == 49)
    ch = ',';

  else if (num == 50)
    ch = ';';

  else if (num == 51)
    ch = ':';

  else if (num == 52)
    ch = '!';

  else if (num == 53)
    ch = '?';

  else if (num == 54)
    ch = '\"';

  else if (num == 55)
    ch = '\'';

  else if (num == 56)
    ch = '`';

  else if (num == 57)
    ch = '-';

  else if (num == 58)
    ch = '+';

  else if (num == 59)
    ch = '\\';

  else if (num == 60)
    ch = '/';

  else if (num == 61)
    ch = '*';

  else if (num == 62)
    ch = '#';

  return ch;
}

inline sc_uint32 sc_addr_to_hash(sc_addr addr)
{
  return SC_ADDR_LOCAL_TO_INT(addr);
}
