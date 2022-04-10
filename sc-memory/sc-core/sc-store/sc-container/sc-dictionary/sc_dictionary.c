/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <stdio.h>
#include <glib.h>
#include <ctype.h>

#include "sc_dictionary.h"
#include "sc_dictionary_private.h"

static const sc_uint8 s_min_sc_char = 1;
static const sc_uint8 s_max_sc_char = 255;

#define SC_DICTIONARY_NODE_IS_VALID(__node) ((__node) != null_ptr)

#define SC_DICTIONARY_NODE_IS_NOT_VALID(__node) ((__node) == null_ptr)

sc_bool sc_dictionary_initialize(sc_dictionary ** dictionary)
{
  if (*dictionary != null_ptr)
    return SC_FALSE;

  *dictionary = g_new0(sc_dictionary, 1);
  (*dictionary)->root = _sc_dictionary_node_initialize();

  return SC_TRUE;
}

sc_bool sc_dictionary_shutdown(sc_dictionary * dictionary)
{
  if (dictionary == null_ptr)
    return SC_FALSE;

  g_free(dictionary);
  return SC_TRUE;
}

inline sc_uint8 _sc_dictionary_children_size()
{
  return s_max_sc_char - s_min_sc_char + 1;
}

inline sc_dictionary_node * _sc_dictionary_node_initialize()
{
  sc_dictionary_node * node = g_new0(sc_dictionary_node, 1);
  node->next = g_new0(sc_dictionary_node *, _sc_dictionary_children_size());

  node->data_list = null_ptr;
  node->mask = 0x0;
  node->offset = null_ptr;
  node->offset_size = 0;

  return node;
}

inline sc_dictionary_node * _sc_dictionary_get_next_node(sc_dictionary_node * node, sc_char ch)
{
  sc_uint8 num;
  sc_char_to_sc_int(ch, &num, &node->mask);

  return num == 0 ? null_ptr : node->next[num];
}

sc_dictionary_node * sc_dictionary_append_to_node(sc_dictionary_node * node, sc_char * sc_string, sc_uint32 size)
{
  sc_char ** sc_string_ptr = g_new0(sc_char *, 1);
  *sc_string_ptr = sc_string;

  sc_uint32 i = 0;
  sc_uint32 saved_offset_size = size;
  while (i < size)
  {
    sc_uint8 num;
    sc_char_to_sc_int(**sc_string_ptr, &num, &node->mask);
    if (SC_DICTIONARY_NODE_IS_NOT_VALID(node->next[num]))
    {
      node->next[num] = _sc_dictionary_node_initialize();

      sc_dictionary_node * temp = node->next[num];

      temp->offset = *sc_string_ptr;
      temp->offset_size = size - i;

      node = temp;

      break;
    }
    else if (node->next[num]->offset != null_ptr)
    {
      sc_dictionary_node * moving = node->next[num];

      sc_uint32 j = 0;
      for (; i < size && j < moving->offset_size && tolower(moving->offset[j]) == tolower(**sc_string_ptr);
           ++i, ++j, ++(*sc_string_ptr))
        ;

      if (j != moving->offset_size)
      {
        saved_offset_size = moving->offset_size;

        node->next[num] = _sc_dictionary_node_initialize();

        sc_dictionary_node * temp = node->next[num];

        temp->offset = moving->offset;
        temp->offset_size = j;
      }

      node = node->next[num];

      if (j < moving->offset_size)
      {
        sc_char * offset_ptr = &*(node->offset + j);

        sc_char_to_sc_int(*offset_ptr, &num, &node->mask);
        if (node->next[num] == null_ptr)
        {
          node->next[num] = moving;

          sc_dictionary_node * temp = node->next[num];

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

sc_dictionary_node * sc_dictionary_append(sc_dictionary * dictionary, sc_char * sc_string, sc_uint32 size, void * value)
{
  sc_dictionary_node * node = sc_dictionary_append_to_node(dictionary->root, sc_string, size);
  if (node->data_list == null_ptr)
    sc_list_init(&node->data_list);

  sc_list_push_back(node->data_list, value, 1);
  return node;
}

sc_dictionary_node * sc_dictionary_remove_from_node(
    sc_dictionary_node * node,
    const sc_char * sc_string,
    sc_uint32 index)
{
  sc_uint32 string_size = strlen(sc_string);
  sc_uint8 num = 0;
  sc_char_to_sc_int(sc_string[index], &num, &node->mask);
  if (num != 0 && SC_DICTIONARY_NODE_IS_VALID(node->next[num]))
  {
    node->next[num] = sc_dictionary_remove_from_node(node->next[num], sc_string, index + node->next[num]->offset_size);

    if (SC_DICTIONARY_NODE_IS_VALID(node->next[num]))
      return node;
  }

  if (index == string_size)
  {
    sc_char * str = g_new0(sc_char, node->offset_size + 1);
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

sc_bool sc_dictionary_remove(sc_dictionary * dictionary, const sc_char * sc_string)
{
  return sc_dictionary_remove_from_node(dictionary->root, sc_string, 0) != null_ptr;
}

sc_dictionary_node * sc_dictionary_get_last_node_from_node(sc_dictionary_node * node, const sc_char * sc_string)
{
  sc_uint32 i = 0;
  sc_uint32 string_size = strlen(sc_string);
  while (i < string_size)
  {
    sc_dictionary_node * next = _sc_dictionary_get_next_node(node, sc_string[i]);
    if (SC_DICTIONARY_NODE_IS_VALID(next))
    {
      node = next;
      i += node->offset_size;
    }
    else
      break;
  }

  if (i == string_size)
  {
    sc_char * str = g_new0(sc_char, node->offset_size + 1);
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

sc_bool sc_dictionary_is_in_from_node(sc_dictionary_node * node, const sc_char * sc_string)
{
  sc_dictionary_node * last = sc_dictionary_get_last_node_from_node(node, sc_string);

  return last != null_ptr && last->data_list != null_ptr;
}

sc_bool sc_dictionary_is_in(sc_dictionary * dictionary, const sc_char * sc_string)
{
  return sc_dictionary_is_in_from_node(dictionary->root, sc_string);
}

void * sc_dictionary_get_data_from_node(sc_dictionary_node * node, const sc_char * sc_string)
{
  sc_dictionary_node * last = sc_dictionary_get_last_node_from_node(node, sc_string);

  if (last != null_ptr && last->data_list != null_ptr && last->data_list->begin != null_ptr)
    return last->data_list->begin->data->value;

  return null_ptr;
}

sc_list * sc_dictionary_get_datas_from_node(sc_dictionary_node * node, const sc_char * sc_string)
{
  sc_dictionary_node * last = sc_dictionary_get_last_node_from_node(node, sc_string);

  if (last != null_ptr)
    return last->data_list;

  return SC_FALSE;
}

void sc_dictionary_show_from_node(sc_dictionary_node * node, sc_char * tab)
{
  sc_uint8 i;
  for (i = 1; i < _sc_dictionary_children_size(); ++i)
  {
    sc_dictionary_node * next = node->next[i];

    if (SC_DICTIONARY_NODE_IS_VALID(next))
    {
      sc_char * str = g_new0(sc_char, next->offset_size + 1);
      memcpy(str, next->offset, next->offset_size);
      sc_uchar ch = sc_int_to_sc_char(i, 0);

      if (next->data_list != null_ptr && next->data_list->size != 0)
        printf("%s%c[%d] {%s}\n", tab, ch, next->data_list->size, str);
      else
        printf("%s%c {%s}\n", tab, ch, str);

      g_free(str);

      sc_char * new_tab = g_new0(sc_char, strlen(tab) + 6);
      strcpy(new_tab, tab);
      strcat(new_tab, "|----\0");

      sc_dictionary_show_from_node(next, new_tab);

      g_free(new_tab);
    }
  }
}

void sc_dictionary_show(sc_dictionary * dictionary)
{
  printf("----------------sc-string tree----------------\n");
  sc_dictionary_show_from_node(dictionary->root, "\0");
  printf("----------------------------------------------\n");
}

void sc_dictionary_visit_node_from_node(
    sc_dictionary_node * node,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_uint8 i;
  for (i = 1; i < _sc_dictionary_children_size(); ++i)
  {
    sc_dictionary_node * next = node->next[i];
    if (next != null_ptr)
    {
      if (next->data_list != null_ptr && next->data_list->size != 0)
        callable(next, dest);

      sc_dictionary_visit_node_from_node(next, callable, dest);
    }
  }
}

void sc_dictionary_visit_nodes(
    sc_dictionary * dictionary,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_dictionary_visit_node_from_node(dictionary->root, callable, dest);
}

inline void sc_char_to_sc_int(sc_char ch, sc_uint8 * ch_num, sc_uint8 * mask)
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

inline sc_char * sc_addr_to_str(sc_addr addr)
{
  sc_uint32 hash = sc_addr_to_hash(addr);

  return itora(hash);
}

inline sc_char * itora(sc_uint32 num)
{
  sc_char * result = g_new0(char, 10);
  sc_char * index = result;

  while (num > 0)
  {
    *index++ = (sc_char)((num % 10) | '0');
    num /= 10;
  }

  return result;
}
