/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_string_tree.h"
#include "stdio.h"

#include <glib.h>
#include <ctype.h>

static const sc_uint8 s_min_sc_char = 1;
static const sc_uint8 s_max_sc_char = 255;

sc_string_tree *tree;
sc_string_tree *links_hashes_tree;

#define SC_STRING_TREE_NODE_IS_VALID(__node) ((__node) != null_ptr)

#define SC_STRING_TREE_NODE_IS_NOT_VALID(__node) ((__node) == null_ptr)

sc_bool sc_string_tree_initialize()
{
  if (_sc_string_tree_links_hashes_initialize() == SC_FALSE)
    return SC_FALSE;

  if (tree != null_ptr)
    return SC_FALSE;

  tree = g_new0(sc_string_tree, 1);
  tree->root = _sc_string_tree_node_initialize();

  return SC_TRUE;
}

sc_bool _sc_string_tree_links_hashes_initialize()
{
  if (links_hashes_tree != null_ptr)
    return SC_FALSE;

  links_hashes_tree = g_new0(sc_string_tree, 1);
  links_hashes_tree->root = _sc_string_tree_node_initialize();
  
  return SC_TRUE;
}

sc_bool sc_string_tree_shutdown()
{
  if (tree == null_ptr)
    return SC_FALSE;

  tree = null_ptr;

  g_free(links_hashes_tree);
  links_hashes_tree = null_ptr;

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

  node->data = null_ptr;
  node->mask = 0x0;
  node->offset = null_ptr;
  node->offset_size = 0;

  return node;
}

inline sc_string_tree_node* _sc_string_tree_get_next_node(sc_string_tree_node *node, sc_char ch)
{
  sc_uint8 num;
  sc_char_to_sc_int(ch, &num, &node->mask);

  if (num == 0)
    return null_ptr;

  return node->next[num];
}

sc_string_tree_node* sc_string_tree_append_to_node(sc_string_tree_node *node, sc_char *sc_string, sc_uint32 size)
{
  sc_uint32 i = 0;
  sc_uint32 saved_offset_size = size;
  while (i < size)
  {
    sc_uint8 num;
    sc_char_to_sc_int(sc_string[i], &num, &node->mask);
    if (SC_STRING_TREE_NODE_IS_NOT_VALID(node->next[num]))
    {
      node->next[num] = _sc_string_tree_node_initialize();

      node->next[num]->offset = &sc_string[i];
      node->next[num]->offset_size = size - i;

      node = node->next[num];

      break;
    }
    else if (node->next[num]->offset != null_ptr)
    {
      sc_string_tree_node *moving = node->next[num];

      sc_uint32 j = 0;
      for (; i < size && j < moving->offset_size && tolower(moving->offset[j]) == tolower(sc_string[i]); ++i, ++j);

      if (j != moving->offset_size)
      {
        saved_offset_size = moving->offset_size;

        node->next[num] = _sc_string_tree_node_initialize();
        node->next[num]->offset = moving->offset;
        node->next[num]->offset_size = j;
      }

      node = node->next[num];

      if (j < moving->offset_size)
      {
        sc_char_to_sc_int(node->offset[j], &num, &node->mask);
        if (node->next[num] == null_ptr)
        {
          node->next[num] = moving;

          node->next[num]->offset = &node->offset[j];
          node->next[num]->offset_size = saved_offset_size - j;
        }
      }
    }
    else
    {
      node = node->next[num];
      ++i;
    }
  }

  return node;
}

sc_string_tree_node* sc_string_tree_append(sc_addr addr, sc_char *sc_string, sc_uint32 size)
{
  sc_link_content *old_content = sc_string_tree_get_content(addr);
  if (old_content != null_ptr && old_content->sc_string != null_ptr && old_content->node != null_ptr)
  {
    sc_addr_hash *hashes = old_content->node->data->value;

    sc_uint8 i;
    for (i = 0; i < old_content->node->data->value_size; ++i)
    {
      sc_addr other;
      other.seg = SC_ADDR_LOCAL_SEG_FROM_INT(hashes[i]);
      other.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(hashes[i]);

      if (SC_ADDR_IS_EQUAL(other, addr))
        ((sc_addr_hash*)old_content->node->data->value)[i] = 0;
    }
  }

  sc_string_tree_node *node = sc_string_tree_append_to_node(tree->root, sc_string, size);

  if (node->data == null_ptr)
  {
    node->data = g_new0(sc_string_tree_node_data, 1);
    node->data->value_size = 0;
    node->data->value = g_new0(sc_addr_hash, ++node->data->value_size);
  }
  else
    node->data->value = g_renew(sc_addr_hash, node->data->value, ++node->data->value_size);

  sc_uint32 hash = sc_addr_to_hash(addr);
  ((sc_addr_hash*)node->data->value)[node->data->value_size - 1] = hash;

  sc_char *hash_str = sc_addr_to_str(addr);
  sc_string_tree_node *link_hash_node
      = sc_string_tree_append_to_node(links_hashes_tree->root, hash_str, strlen(hash_str));

  if (link_hash_node->data == null_ptr)
  {
    link_hash_node->data = g_new0(sc_string_tree_node_data, 1);
    link_hash_node->data->value_size = 1;
  }

  if (link_hash_node->data->value != null_ptr)
    g_free(link_hash_node->data->value);

  sc_link_content *content = g_new0(sc_link_content, 1);

  content->sc_string = g_new0(sc_char, size);
  content->string_size = size;
  memcpy(content->sc_string, sc_string, size);

  content->node = &(*node);
  link_hash_node->data->value = content;

  return node;
}

sc_string_tree_node* sc_string_tree_remove_from_node(sc_string_tree_node *node, const sc_char *sc_string, sc_uint32 index)
{
  sc_uint32 string_size = strlen(sc_string);
  sc_uint8 num = 0;
  sc_char_to_sc_int(sc_string[index], &num, &node->mask);
  if (num != 0 && SC_STRING_TREE_NODE_IS_VALID(node->next[num]))
  {
    node->next[num] = sc_string_tree_remove_from_node(node->next[num], sc_string, index + node->next[num]->offset_size);

    if (SC_STRING_TREE_NODE_IS_VALID(node->next[num]))
      return node;
  }

  if (node->data != null_ptr && index == string_size)
    return null_ptr;

  return node;
}

sc_bool sc_string_tree_remove(const sc_char *sc_string)
{
  return sc_string_tree_remove_from_node(tree->root, sc_string, 0) != null_ptr;
}

sc_string_tree_node* sc_string_tree_get_last_node_from_node(sc_string_tree_node *node, const sc_char *sc_string)
{
  sc_uint32 i;
  sc_uint32 string_size = strlen(sc_string);
  for (i = 0; i < string_size;)
  {
    sc_string_tree_node *next = _sc_string_tree_get_next_node(node, sc_string[i]);
    if (SC_STRING_TREE_NODE_IS_VALID(next))
    {
      node = next;
      i += node->offset_size;
    }
    else
      break;
  }

  if (i == string_size)
    return node;

  return null_ptr;
}

sc_bool sc_string_tree_is_in_from_node(sc_string_tree_node *node, const sc_char *sc_string)
{
  sc_string_tree_node *last = sc_string_tree_get_last_node_from_node(node, sc_string);

  return last != null_ptr && last->data != null_ptr;
}

sc_bool sc_string_tree_is_in(const sc_char *sc_string)
{
  return sc_string_tree_is_in_from_node(tree->root, sc_string);
}

void* sc_string_tree_get_data_from_node(sc_string_tree_node *node, const sc_char *sc_string)
{
  sc_string_tree_node *last = sc_string_tree_get_last_node_from_node(node, sc_string);

  if (last != null_ptr && last->data != null_ptr)
    return last->data->value;

  return null_ptr;
}

sc_addr sc_string_tree_get_sc_link(const sc_char *sc_string)
{
  sc_addr_hash *addr_hash = sc_string_tree_get_data_from_node(tree->root, sc_string);
  if (addr_hash != null_ptr && *addr_hash != 0)
  {
    sc_addr addr;
    addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(*addr_hash);
    addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(*addr_hash);
    return addr;
  }

  sc_addr empty;
  SC_ADDR_MAKE_EMPTY(empty)
  return empty;
}

sc_bool sc_string_tree_get_datas_from_node(sc_string_tree_node *node, const sc_char *sc_string, void **data, sc_uint32 *size)
{
  sc_string_tree_node *last = sc_string_tree_get_last_node_from_node(node, sc_string);

  if (last != null_ptr && last->data != null_ptr && last->data->value_size != 0)
  {
    *data = last->data->value;
    *size = last->data->value_size;

    return SC_TRUE;
  }

  *data = null_ptr;
  *size = 0;
  return SC_FALSE;
}

sc_bool sc_string_tree_get_sc_links(const sc_char *sc_string, sc_addr **links, sc_uint32 *size)
{
  sc_addr_hash * addr_hashes;
  sc_uint32 found_size = 0;
  sc_bool result = sc_string_tree_get_datas_from_node(tree->root, sc_string, (void **) &addr_hashes, &found_size);

  *links = null_ptr;
  *size = 0;
  sc_uint32 i;
  for (i = 0; i < found_size; ++i)
  {
    sc_addr addr;
    addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(addr_hashes[i]);
    addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(addr_hashes[i]);

    if (SC_ADDR_IS_NOT_EMPTY(addr))
    {
      if (*links == null_ptr)
        *links = g_new0(sc_addr, ++*size);
      else
        *links = g_renew(sc_addr, *links, ++*size);

      (*links)[*size - 1] = addr;
    }
  }

  return result;
}

sc_char* sc_string_tree_get_sc_string(sc_addr addr)
{
  if (SC_ADDR_IS_EMPTY(addr))
    return null_ptr;

  sc_link_content *content = sc_string_tree_get_data_from_node(links_hashes_tree->root, sc_addr_to_str(addr));

  if (content == null_ptr || content->sc_string == null_ptr)
    return null_ptr;

  sc_uint32 len = content->string_size;
  sc_char *copy = g_new0(sc_char, len);
  return memcpy(copy, content->sc_string, len);
}

sc_string_tree_node* sc_string_tree_get_node(sc_addr addr)
{
  if (SC_ADDR_IS_EMPTY(addr))
    return null_ptr;

  sc_link_content *content = sc_string_tree_get_data_from_node(links_hashes_tree->root, sc_addr_to_str(addr));

  if (content == null_ptr || content->node == null_ptr)
    return null_ptr;

  return content->node;
}

sc_link_content* sc_string_tree_get_content(sc_addr addr)
{
  if (SC_ADDR_IS_EMPTY(addr))
    return null_ptr;

  sc_link_content *content = sc_string_tree_get_data_from_node(links_hashes_tree->root, sc_addr_to_str(addr));

  if (content == null_ptr || content->node == null_ptr || content->sc_string == null_ptr)
    return null_ptr;

  return content;
}

void sc_string_tree_get_sc_string_ext(sc_addr addr, sc_char **sc_string, sc_uint32 *size)
{
  if (SC_ADDR_IS_EMPTY(addr))
  {
    *sc_string = null_ptr;
    *size = 0;
  }

  sc_link_content *content = sc_string_tree_get_data_from_node(links_hashes_tree->root, sc_addr_to_str(addr));

  *sc_string = content->sc_string;
  *size = content->string_size;
}

void sc_string_tree_show_from_node(sc_string_tree_node *node, sc_char *tab)
{
  sc_uint8 i;
  for (i = 1; i < sc_string_tree_children_size(); ++i)
  {
    if (node->next[i] != null_ptr)
    {
      sc_string_tree_node *next = node->next[i];
      sc_char *str = g_new0(sc_char, node->next[i]->offset_size);
      memcpy(str, node->next[i]->offset, node->next[i]->offset_size);
      sc_uchar ch = sc_int_to_sc_char(i, 0);

      if (next->data != null_ptr && next->data->value_size != 0)
        printf("%s%c[%d] {%s}\n", tab, ch, next->data->value_size, str);
      else
        printf("%s%c {%s}\n", tab, ch, str);

      sc_char *new_tab = g_new0(sc_char, strlen(tab) + 6);
      strcpy(new_tab, tab);
      strcat(new_tab, "|----\0");

      sc_string_tree_show_from_node(next, new_tab);
    }
  }
}

void sc_string_tree_show()
{
  printf("----------------sc-string tree----------------\n");
  sc_string_tree_show_from_node(tree->root, "\0");
  printf("----------------------------------------------\n");
}

void sc_string_tree_visit_node_from_node(sc_string_tree_node *node, void (*callable)(sc_string_tree_node*, void*), void *dest)
{
  sc_uint8 i;
  for (i = 1; i < sc_string_tree_children_size(); ++i)
  {
    if (node->next[i] != null_ptr)
    {
      sc_string_tree_node *next = node->next[i];

      if (next->data != null_ptr && next->data->value != null_ptr && next->data->value_size != 0)
        callable(next, dest);

      sc_string_tree_visit_node_from_node(next, callable, dest);
    }
  }
}

void sc_string_tree_write_nodes(void (*callable)(sc_string_tree_node*, void*), void *dest)
{
  sc_string_tree_visit_node_from_node(links_hashes_tree->root, callable, dest);
}

void sc_string_tree_write_node(sc_string_tree_node *node, void *dest)
{
  FILE *file = dest;
  sc_link_content *content = node->data->value;

  if (content->node->mask & 0xF0)
    return;

  sc_addr_hash *hashes = content->node->data->value;
  sc_uint8 hashes_size = content->node->data->value_size;

  content->node->mask |= 0xF0;

  sc_uint32 i;
  for (i = 0; i < hashes_size; ++i)
  {
    if (hashes[i] != 0)
      fprintf(file, "%d :: \"%s\"\n", hashes[i], content->sc_string);
  }
}

void sc_string_tree_links_hashes_show()
{
  sc_string_tree_show_from_node(links_hashes_tree->root, "\0");
}

inline void sc_char_to_sc_int(sc_uchar ch, sc_uint8 *ch_num, sc_uint8 *mask)
{
  *ch_num = (sc_uint8)ch;
}

inline sc_uchar sc_int_to_sc_char(sc_uint8 num, sc_uint8 mask)
{
  return (sc_uchar)num;
}

inline sc_uint32 sc_addr_to_hash(sc_addr addr)
{
  return SC_ADDR_LOCAL_TO_INT(addr);
}

inline sc_char* sc_addr_to_str(sc_addr addr)
{
  sc_uint32 hash = sc_addr_to_hash(addr);

  return itora(hash);
}

inline sc_char* itora(sc_uint32 num)
{
  sc_char *result = g_new0(char, 10);
  sc_char *index = result;

  while (num > 0)
  {
    *index++ = (sc_char)((num % 10) | '0');
    num /= 10;
  }

  sc_char *end = result + 10;
  while (index != end)
    *index++ = '\0';

  return result;
}
