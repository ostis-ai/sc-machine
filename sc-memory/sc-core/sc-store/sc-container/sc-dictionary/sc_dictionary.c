/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <ctype.h>

#include "sc_dictionary.h"
#include "sc_dictionary_private.h"

#include "../../sc-base/sc_allocator.h"
#include "../../sc-container/sc-string/sc_string.h"

#define SC_DICTIONARY_NODE_IS_VALID(__node) ((__node) != null_ptr)
#define SC_DICTIONARY_NODE_IS_NOT_VALID(__node) ((__node) == null_ptr)

sc_bool sc_dictionary_initialize(
    sc_dictionary ** dictionary,
    sc_uint8 children_size,
    void (*char_to_int)(sc_char, sc_uint8 *, sc_uint8 *))
{
  if (*dictionary != null_ptr)
    return SC_FALSE;

  *dictionary = sc_mem_new(sc_dictionary, 1);
  (*dictionary)->size = children_size;
  (*dictionary)->root = _sc_dictionary_node_initialize(children_size);
  (*dictionary)->char_to_int = char_to_int;

  return SC_TRUE;
}

sc_bool sc_dictionary_destroy(sc_dictionary * dictionary, void (*node_destroy)(sc_dictionary_node *, void **))
{
  if (dictionary == null_ptr)
    return SC_FALSE;

  sc_dictionary_visit_up_nodes(dictionary, node_destroy, null_ptr);
  node_destroy(dictionary->root, null_ptr);
  sc_mem_free(dictionary);

  return SC_TRUE;
}

inline sc_dictionary_node * _sc_dictionary_node_initialize(sc_uint8 children_size)
{
  sc_dictionary_node * node = sc_mem_new(sc_dictionary_node, 1);
  node->next = sc_mem_new(sc_dictionary_node *, children_size);

  node->data_list = null_ptr;
  node->offset = null_ptr;
  node->offset_size = 0;

  sc_dc_node_access_lvl_make_read(node);

  return node;
}

void sc_dictionary_node_destroy(sc_dictionary_node * node, void ** args)
{
  (void)args;

  sc_list_clear(node->data_list);
  sc_list_destroy(node->data_list);
  node->data_list = null_ptr;

  sc_mem_free(node->next);
  node->next = null_ptr;

  sc_mem_free(node->offset);
  node->offset = null_ptr;
  node->offset_size = 0;

  sc_mem_free(node);
}

inline sc_dictionary_node * _sc_dictionary_get_next_node(
    sc_dictionary * dictionary,
    sc_dictionary_node * node,
    sc_char ch)
{
  sc_uint8 num;
  dictionary->char_to_int(ch, &num, &node->mask);

  return node->next == null_ptr ? null_ptr : node->next[num];
}

sc_dictionary_node * sc_dictionary_append_to_node(sc_dictionary * dictionary, const sc_char * sc_string, sc_uint32 size)
{
  sc_dictionary_node * node = dictionary->root;

  sc_char ** sc_string_ptr = sc_mem_new(sc_char *, 1);
  *sc_string_ptr = (sc_char *)sc_string;

  sc_uint32 i = 0;
  sc_uint32 saved_offset_size = size;
  while (i < size)
  {
    sc_uint8 num;
    dictionary->char_to_int(**sc_string_ptr, &num, &node->mask);
    // define prefix
    if (SC_DICTIONARY_NODE_IS_NOT_VALID(node->next[num]))
    {
      node->next[num] = _sc_dictionary_node_initialize(dictionary->size);

      sc_dictionary_node * temp = node->next[num];

      temp->offset_size = size - i;
      sc_str_cpy(temp->offset, *sc_string_ptr, temp->offset_size);

      node = temp;

      break;
    }
    // visit next substring
    else if (node->next[num]->offset != null_ptr)
    {
      sc_dictionary_node * moving = node->next[num];

      sc_uint32 j = 0;
      for (; i < size && j < moving->offset_size && tolower(moving->offset[j]) == tolower(**sc_string_ptr);
           ++i, ++j, ++*sc_string_ptr)
        ;

      if (j != moving->offset_size)
      {
        saved_offset_size = moving->offset_size;

        node->next[num] = _sc_dictionary_node_initialize(dictionary->size);

        sc_dictionary_node * temp = node->next[num];

        temp->offset_size = j;
        temp->offset = moving->offset;
      }

      node = node->next[num];

      // insert intermediate node for prefix end branching
      if (j < moving->offset_size)
      {
        sc_char * offset_ptr = &*(moving->offset + j);

        dictionary->char_to_int(*offset_ptr, &num, &node->mask);
        node->next[num] = &*moving;

        sc_dictionary_node * temp = node->next[num];

        temp->offset_size = saved_offset_size - j;
        sc_str_cpy(temp->offset, offset_ptr, temp->offset_size);
      }
    }
    else
    {
      node = node->next[num];
      ++*sc_string_ptr;
      ++i;
    }
  }

  sc_mem_free(sc_string_ptr);
  return node;
}

sc_dictionary_node * sc_dictionary_append(
    sc_dictionary * dictionary,
    const sc_char * sc_string,
    sc_uint32 size,
    void * value)
{
  sc_dictionary_node * node = sc_dictionary_append_to_node(dictionary, sc_string, size);

  if (node->data_list == null_ptr)
    sc_list_init(&node->data_list);

  void * copy = sc_mem_new(void, sizeof(value));
  *(void **)copy = *(void **)value;
  sc_list_push_back(node->data_list, copy);
  return node;
}

sc_dictionary_node * sc_dictionary_remove_from_node(
    sc_dictionary * dictionary,
    sc_dictionary_node * node,
    const sc_char * sc_string,
    sc_uint32 index)
{
  // check prefixes matching
  sc_uint32 string_size = strlen(sc_string);

  if (index < string_size)
  {
    sc_uint8 num = 0;
    dictionary->char_to_int(sc_string[index], &num, &node->mask);
    if (SC_DICTIONARY_NODE_IS_VALID(node->next[num]))
    {
      sc_dictionary_node * next = node->next[num];
      sc_dictionary_node * removable =
          sc_dictionary_remove_from_node(dictionary, next, sc_string, index + next->offset_size);

      if (SC_DICTIONARY_NODE_IS_VALID(next))
        return removable;
    }
  }

  // check suffixes matching
  if (index == string_size)
  {
    if (strcmp(node->offset, sc_string + (string_size - node->offset_size)) == 0)
      return node;
  }

  return null_ptr;
}

sc_bool sc_dictionary_remove(sc_dictionary * dictionary, const sc_char * sc_string)
{
  sc_dictionary_node * node = sc_dictionary_remove_from_node(dictionary, dictionary->root, sc_string, 0);

  sc_bool result = node != null_ptr;
  if (result == SC_TRUE)
  {
    sc_list_destroy(node->data_list);
    node->data_list = null_ptr;
  }

  return result;
}

sc_dictionary_node * sc_dictionary_get_last_node_from_node(
    sc_dictionary * dictionary,
    sc_dictionary_node * node,
    const sc_char * sc_string)
{
  if (sc_string == null_ptr)
    return null_ptr;

  // check prefixes matching
  sc_uint32 i = 0;
  sc_uint32 string_size = strlen(sc_string);
  while (i < string_size)
  {
    sc_dictionary_node * next = _sc_dictionary_get_next_node(dictionary, node, sc_string[i]);
    if (SC_DICTIONARY_NODE_IS_VALID(next))
    {
      node = next;
      i += node->offset_size;
    }
    else
      break;
  }

  // check suffixes matching
  if (i == string_size)
  {
    sc_char * str = sc_mem_new(sc_char, node->offset_size + 1);
    sc_mem_cpy(str, node->offset, node->offset_size);

    if (strcmp(str, sc_string + (string_size - node->offset_size)) == 0)
    {
      sc_mem_free(str);
      return node;
    }
    sc_mem_free(str);
  }

  return null_ptr;
}

sc_bool sc_dictionary_is_in(sc_dictionary * dictionary, const sc_char * sc_string)
{
  sc_dictionary_node * last = sc_dictionary_get_last_node_from_node(dictionary, dictionary->root, sc_string);

  return SC_DICTIONARY_NODE_IS_VALID(last) && last->data_list != null_ptr;
}

void * sc_dictionary_get_first_data_from_node(
    sc_dictionary * dictionary,
    sc_dictionary_node * node,
    const sc_char * sc_string)
{
  sc_dictionary_node * last = sc_dictionary_get_last_node_from_node(dictionary, node, sc_string);

  if (SC_DICTIONARY_NODE_IS_VALID(last) && last->data_list != null_ptr && last->data_list->begin != null_ptr)
    return last->data_list->begin->data;

  return null_ptr;
}

sc_list * sc_dictionary_get(sc_dictionary * dictionary, const sc_char * sc_string)
{
  sc_dictionary_node * last = sc_dictionary_get_last_node_from_node(dictionary, dictionary->root, sc_string);

  if (SC_DICTIONARY_NODE_IS_VALID(last))
    return last->data_list;

  return null_ptr;
}

void sc_dictionary_visit_down_node_from_node(
    sc_dictionary * dictionary,
    sc_dictionary_node * node,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_uint8 i;
  for (i = 0; i < dictionary->size; ++i)
  {
    sc_dictionary_node * next = node->next[i];
    if (SC_DICTIONARY_NODE_IS_VALID(next))
    {
      callable(next, dest);

      sc_dictionary_visit_down_node_from_node(dictionary, next, callable, dest);
    }
  }
}

void sc_dictionary_visit_down_nodes(
    sc_dictionary * dictionary,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_dictionary_visit_down_node_from_node(dictionary, dictionary->root, callable, dest);
}

void sc_dictionary_visit_up_node_from_node(
    sc_dictionary * dictionary,
    sc_dictionary_node * node,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_uint8 i;
  for (i = 0; i < dictionary->size; ++i)
  {
    sc_dictionary_node * next = node->next[i];
    if (SC_DICTIONARY_NODE_IS_VALID(next))
    {
      sc_dictionary_visit_up_node_from_node(dictionary, next, callable, dest);

      callable(next, dest);
    }
  }
}

void sc_dictionary_visit_up_nodes(
    sc_dictionary * dictionary,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_dictionary_visit_up_node_from_node(dictionary, dictionary->root, callable, dest);
}
