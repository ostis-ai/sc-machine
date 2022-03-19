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

  g_free(tree);
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

  return num == 0 ? null_ptr : node->next[num];
}

sc_string_tree_node* sc_string_tree_append_to_node(sc_string_tree_node *node, sc_char *sc_string, sc_uint32 size)
{
  sc_char **sc_string_ptr = g_new0(sc_char*, 1);
  *sc_string_ptr = sc_string;

  sc_uint32 i = 0;
  sc_uint32 saved_offset_size = size;
  while (i < size)
  {
    sc_uint8 num;
    sc_char_to_sc_int(**sc_string_ptr, &num, &node->mask);
    if (SC_STRING_TREE_NODE_IS_NOT_VALID(node->next[num]))
    {
      node->next[num] = _sc_string_tree_node_initialize();

      sc_string_tree_node *temp = node->next[num];

      temp->offset = *sc_string_ptr;
      temp->offset_size = size - i;

      node = temp;

      break;
    }
    else if (node->next[num]->offset != null_ptr)
    {
      sc_string_tree_node *moving = node->next[num];

      sc_uint32 j = 0;
      for (; i < size && j < moving->offset_size && tolower(moving->offset[j]) == tolower(**sc_string_ptr);
          ++i, ++j, ++(*sc_string_ptr));

      if (j != moving->offset_size)
      {
        saved_offset_size = moving->offset_size;

        node->next[num] = _sc_string_tree_node_initialize();

        sc_string_tree_node *temp = node->next[num];

        temp->offset = moving->offset;
        temp->offset_size = j;
      }

      node = node->next[num];

      if (j < moving->offset_size)
      {
        sc_char *offset_ptr = &*(node->offset + j);

        sc_char_to_sc_int(*offset_ptr, &num, &node->mask);
        if (node->next[num] == null_ptr)
        {
          node->next[num] = moving;

          sc_string_tree_node *temp = node->next[num];

          temp->offset = offset_ptr;
          temp->offset_size = saved_offset_size - j;
        }
      }
    }
    else
    {
      node = node->next[num];
      (*sc_string_ptr)++;
      ++i;
    }
  }

  g_free(sc_string_ptr);
  return node;
}

sc_string_tree_node* sc_string_tree_append(sc_addr addr, sc_char *sc_string, sc_uint32 size)
{
  sc_link_content *old_content = sc_string_tree_get_content(addr);
  if (old_content != null_ptr && old_content->sc_string != null_ptr && old_content->node != null_ptr)
  {
    sc_uint8 hashes_size = old_content->node->data->value_size;
    sc_addr_hash *hashes = old_content->node->data->value;

    sc_addr_hash **temp = g_new0(sc_addr_hash*, 1);
    *temp = old_content->node->data->value;

    sc_uint8 i = 0;
    while (i++ < hashes_size)
    {
      sc_addr_hash hash = *hashes;

      sc_addr other;
      other.seg = SC_ADDR_LOCAL_SEG_FROM_INT(hash);
      other.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(hash);

      if (SC_ADDR_IS_EQUAL(other, addr))
        *hashes = 0;

      hashes++;
    }

    old_content->node->data->value = *temp;
    g_free(temp);
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

  sc_addr_hash hash = sc_addr_to_hash(addr);
  ((sc_addr_hash*)node->data->value)[node->data->value_size - 1] = hash;

  sc_char *hash_str = sc_addr_to_str(addr);
  sc_addr_hash hash_str_len = strlen(hash_str);
  sc_string_tree_node *link_hash_node
      = sc_string_tree_append_to_node(links_hashes_tree->root, hash_str, hash_str_len);

  if (link_hash_node->data == null_ptr)
  {
    link_hash_node->data = g_new0(sc_string_tree_node_data, 1);
    link_hash_node->data->value_size = 1;
  }

  if (link_hash_node->data->value != null_ptr)
    g_free(link_hash_node->data->value);

  sc_link_content *content = g_new0(sc_link_content, 1);

  content->sc_string = g_new0(sc_char, size + 1);
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

  if (index == string_size)
  {
    sc_char *str = g_new0(sc_char, node->offset_size + 1);
    memcpy(str, node->offset, node->offset_size);

    if (strcmp(str, sc_string + (string_size - node->offset_size)) == 0)
    {
      g_free(str);
      return null_ptr;
    }
    g_free(str);
  }

  return node;
}

sc_bool sc_string_tree_remove(const sc_char *sc_string)
{
  return sc_string_tree_remove_from_node(tree->root, sc_string, 0) != null_ptr;
}

sc_string_tree_node* sc_string_tree_get_last_node_from_node(sc_string_tree_node *node, const sc_char *sc_string)
{
  sc_uint32 i = 0;
  sc_uint32 string_size = strlen(sc_string);
  while (i < string_size)
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
  {
    sc_char *str = g_new0(sc_char, node->offset_size + 1);
    memcpy(str, node->offset, node->offset_size);

    if (strcmp(str, sc_string + (string_size - node->offset_size)) == 0)
    {
      g_free(str);
      return node;
    }
    g_free(str);
  }

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
  sc_addr empty;
  SC_ADDR_MAKE_EMPTY(empty)

  sc_addr_hash *addr_hash = sc_string_tree_get_data_from_node(tree->root, sc_string);
  if (addr_hash == null_ptr)
    return empty;

  if (*addr_hash != 0)
  {
    sc_addr addr;
    addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(*addr_hash);
    addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(*addr_hash);
    return addr;
  }

  while (*addr_hash == 0)
  {
    if (*addr_hash != 0)
    {
      sc_addr addr;
      addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(*addr_hash);
      addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(*addr_hash);
    }

    addr_hash++;
  }

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
  sc_addr_hash *addr_hashes = null_ptr;
  sc_uint32 found_size = 0;
  sc_bool result = sc_string_tree_get_datas_from_node(tree->root, sc_string, (void**)&addr_hashes, &found_size);

  sc_addr **temps = null_ptr;
  *links = null_ptr;
  *size = 0;
  sc_uint32 i = 0;

  *links = g_new0(sc_addr, found_size);
  temps = g_new0(sc_addr*, 1);
  *temps = *links;

  while (i++ < found_size)
  {
    sc_addr addr;
    sc_addr_hash hash = *addr_hashes++;
    addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(hash);
    addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(hash);

    if (SC_ADDR_IS_NOT_EMPTY(addr))
    {
      **temps = addr;
      ++*temps;
      ++*size;
    }
  }

  *links = memcpy(*links, *links, *size);
  g_free(temps);

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
  sc_char *copy = g_new0(sc_char, len + 1);
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
    return;
  }

  sc_link_content *content = sc_string_tree_get_data_from_node(links_hashes_tree->root, sc_addr_to_str(addr));
  if (content == null_ptr)
  {
    *sc_string = null_ptr;
    *size = 0;
    return;
  }

  sc_uint32 len = content->string_size;
  *sc_string = g_new0(sc_char, len + 1);
  memcpy(*sc_string, content->sc_string, len);
  *size = len;
}

void sc_string_tree_show_from_node(sc_string_tree_node *node, sc_char *tab)
{
  sc_uint8 i;
  for (i = 1; i < sc_string_tree_children_size(); ++i)
  {
    sc_string_tree_node *next = node->next[i];

    if (SC_STRING_TREE_NODE_IS_VALID(next))
    {
      sc_char *str = g_new0(sc_char, next->offset_size + 1);
      memcpy(str, next->offset, next->offset_size);
      sc_uchar ch = sc_int_to_sc_char(i, 0);

      if (next->data != null_ptr && next->data->value_size != 0)
        printf("%s%c[%d] {%s}\n", tab, ch, next->data->value_size, str);
      else
        printf("%s%c {%s}\n", tab, ch, str);

      g_free(str);

      sc_char *new_tab = g_new0(sc_char, strlen(tab) + 6);
      strcpy(new_tab, tab);
      strcat(new_tab, "|----\0");

      sc_string_tree_show_from_node(next, new_tab);

      g_free(new_tab);
    }
  }
}

void sc_string_tree_show()
{
  printf("----------------sc-string tree----------------\n");
  sc_string_tree_show_from_node(tree->root, "\0");
  printf("----------------------------------------------\n");
}

void sc_string_tree_visit_node_from_node(sc_string_tree_node *node, void (*callable)(sc_string_tree_node*, void**), void **dest)
{
  sc_uint8 i;
  for (i = 1; i < sc_string_tree_children_size(); ++i)
  {
    sc_string_tree_node *next = node->next[i];
    if (next != null_ptr)
    {
      if (next->data != null_ptr && next->data->value != null_ptr && next->data->value_size != 0)
        callable(next, dest);

      sc_string_tree_visit_node_from_node(next, callable, dest);
    }
  }
}

void sc_string_tree_visit_nodes(void (*callable)(sc_string_tree_node*, void**), void **dest)
{
  sc_string_tree_visit_node_from_node(links_hashes_tree->root, callable, dest);
}

void sc_string_tree_links_hashes_show()
{
  sc_string_tree_show_from_node(links_hashes_tree->root, "\0");
}

inline void sc_char_to_sc_int(sc_char ch, sc_uint8 *ch_num, sc_uint8 *mask)
{
  *ch_num = 128 + (sc_uint8)ch;
}

inline sc_char sc_int_to_sc_char(sc_uint8 num, sc_uint8 mask)
{
  return (sc_char)(num - 128);
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

  return result;
}
