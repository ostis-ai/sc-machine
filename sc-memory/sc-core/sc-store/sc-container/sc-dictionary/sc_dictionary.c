/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_dictionary.h"
#include "sc_dictionary_private.h"

#include "../../sc-base/sc_allocator.h"
#include "../../sc-container/sc-string/sc_string.h"

#define SC_DICTIONARY_NODE_BUCKET_SIZE 16
#define SC_DICTIONARY_GET_BUCKET_NUM(num) ((num) / SC_DICTIONARY_NODE_BUCKET_SIZE)
#define SC_DICTIONARY_GET_CHILD_NUM(num) ((num) % SC_DICTIONARY_NODE_BUCKET_SIZE)

#define SC_DICTIONARY_NODE_IS_VALID(__node) ((__node) != null_ptr)
#define SC_DICTIONARY_NODE_IS_NOT_VALID(__node) ((__node) == null_ptr)

sc_bool sc_dictionary_initialize(
    sc_dictionary ** dictionary,
    sc_uint8 children_size,
    void (*char_to_int)(sc_char, sc_uint8 *, const sc_uint8 *))
{
  *dictionary = sc_mem_new(sc_dictionary, 1);
  (*dictionary)->size = children_size;
  (*dictionary)->root = _sc_dictionary_node_initialize(children_size);
  (*dictionary)->char_to_int = char_to_int;

  return SC_TRUE;
}

inline sc_dictionary_node * _sc_dictionary_node_initialize(sc_uint8 children_size)
{
  sc_dictionary_node * node = sc_mem_new(sc_dictionary_node, 1);

  sc_uint64 const count = children_size / SC_DICTIONARY_NODE_BUCKET_SIZE + 1;
  node->next = sc_mem_new(sc_dictionary_node **, count);

  node->data = null_ptr;
  node->offset = null_ptr;
  node->offset_size = 0;

  return node;
}

void _sc_dictionary_node_destroy(sc_dictionary_node * node)
{
  node->data = null_ptr;

  sc_mem_free(node->next);
  node->next = null_ptr;

  sc_mem_free(node->offset);
  node->offset = null_ptr;
  node->offset_size = 0;

  sc_mem_free(node);
}

void _sc_dictionary_up_destroy_node(
    sc_dictionary const * dictionary,
    sc_dictionary_node * node,
    void (*node_clear)(sc_dictionary_node *))
{
  sc_dictionary_node ** next_bucket = null_ptr;
  for (sc_uint8 num = 0, prev_bucket_idx = 0; num < dictionary->size; ++num)
  {
    sc_uint64 const bucket_idx = SC_DICTIONARY_GET_BUCKET_NUM(num);
    if (prev_bucket_idx != bucket_idx)
      sc_mem_free(next_bucket);
    prev_bucket_idx = bucket_idx;

    next_bucket = node->next[bucket_idx];
    if (SC_DICTIONARY_NODE_IS_NOT_VALID(next_bucket))
      continue;

    sc_uint64 const child_idx = SC_DICTIONARY_GET_CHILD_NUM(num);
    sc_dictionary_node * next = next_bucket[child_idx];
    if (SC_DICTIONARY_NODE_IS_NOT_VALID(next))
      continue;

    _sc_dictionary_up_destroy_node(dictionary, next, node_clear);

    if (node_clear != null_ptr)
      node_clear(next);
    _sc_dictionary_node_destroy(next);
  }

  sc_mem_free(next_bucket);
}

sc_bool sc_dictionary_destroy(sc_dictionary * dictionary, void (*node_clear)(sc_dictionary_node *))
{
  if (dictionary == null_ptr)
    return SC_FALSE;

  _sc_dictionary_up_destroy_node(dictionary, dictionary->root, node_clear);

  if (node_clear != null_ptr)
    node_clear(dictionary->root);
  _sc_dictionary_node_destroy(dictionary->root);

  sc_mem_free(dictionary);

  return SC_TRUE;
}

inline sc_dictionary_node * _sc_dictionary_get_next_node(
    sc_dictionary const * dictionary,
    sc_dictionary_node const * node,
    sc_char const ch)
{
  sc_uint8 num;
  dictionary->char_to_int(ch, &num, &node->mask);

  sc_uint64 const bucket_idx = SC_DICTIONARY_GET_BUCKET_NUM(num);
  sc_uint64 const child_idx = SC_DICTIONARY_GET_CHILD_NUM(num);

  return node->next == null_ptr ? null_ptr : (node->next[bucket_idx] == null_ptr ? null_ptr : node->next[bucket_idx][child_idx]);
}

sc_dictionary_node * sc_dictionary_append_to_node(sc_dictionary * dictionary, sc_char const * string, sc_uint32 size)
{
  sc_dictionary_node * node = dictionary->root;
  sc_char * string_ptr = (sc_char *)&*string;

  sc_uint32 i = 0;
  sc_uint32 saved_offset_size = size;
  while (i < size)
  {
    sc_uint8 num;
    dictionary->char_to_int(*string_ptr, &num, &node->mask);
    sc_uint64 bucket_idx = SC_DICTIONARY_GET_BUCKET_NUM(num);
    sc_uint64 child_idx = SC_DICTIONARY_GET_CHILD_NUM(num);

    if (SC_DICTIONARY_NODE_IS_NOT_VALID(node->next[bucket_idx]))
      node->next[bucket_idx] = sc_mem_new(sc_dictionary_node *, SC_DICTIONARY_NODE_BUCKET_SIZE);

    // define prefix
    if (SC_DICTIONARY_NODE_IS_NOT_VALID(node->next[bucket_idx][child_idx]))
    {
      node->next[bucket_idx][child_idx] = _sc_dictionary_node_initialize(dictionary->size);

      sc_dictionary_node * temp = node->next[bucket_idx][child_idx];

      temp->offset_size = size - i;
      sc_str_cpy(temp->offset, string_ptr, temp->offset_size);

      node = temp;

      break;
    }
    // visit next substring
    else if (node->next[bucket_idx][child_idx]->offset != null_ptr)
    {
      sc_dictionary_node * moving = node->next[bucket_idx][child_idx];

      sc_uint32 j = 0;
      for (; i < size && j < moving->offset_size && moving->offset[j] == *string_ptr; ++i, ++j, ++string_ptr)
        ;

      if (j != moving->offset_size)
      {
        saved_offset_size = moving->offset_size;

        node->next[bucket_idx][child_idx] = _sc_dictionary_node_initialize(dictionary->size);

        sc_dictionary_node * temp = node->next[bucket_idx][child_idx];

        temp->offset_size = j;
        sc_str_cpy(temp->offset, moving->offset, temp->offset_size);
      }
      node = node->next[bucket_idx][child_idx];

      // insert intermediate node for prefix end branching
      if (j < moving->offset_size)
      {
        sc_char * offset_ptr = &*(moving->offset + j);

        dictionary->char_to_int(*offset_ptr, &num, &node->mask);
        bucket_idx = SC_DICTIONARY_GET_BUCKET_NUM(num);
        child_idx = SC_DICTIONARY_GET_CHILD_NUM(num);

        sc_char * moving_offset_copy = moving->offset;

        if (SC_DICTIONARY_NODE_IS_NOT_VALID(node->next[bucket_idx]))
          node->next[bucket_idx] = sc_mem_new(sc_dictionary_node *, SC_DICTIONARY_NODE_BUCKET_SIZE);

        node->next[bucket_idx][child_idx] = &*moving;

        sc_dictionary_node * temp = node->next[bucket_idx][child_idx];

        temp->offset_size = saved_offset_size - j;
        sc_str_cpy(temp->offset, offset_ptr, temp->offset_size);
        sc_mem_free(moving_offset_copy);
      }
    }
    else
    {
      node = node->next[bucket_idx][child_idx];
      ++string_ptr;
      ++i;
    }
  }

  return node;
}

sc_dictionary_node * sc_dictionary_append(
    sc_dictionary * dictionary,
    const sc_char * string,
    sc_uint32 size,
    void * value)
{
  sc_dictionary_node * node = sc_dictionary_append_to_node(dictionary, string, size);
  node->data = value;
  return node;
}

const sc_dictionary_node * sc_dictionary_get_last_node_from_node(
    const sc_dictionary * dictionary,
    const sc_dictionary_node * node,
    const sc_char * string,
    const sc_uint32 string_size)
{
  // check prefixes matching
  sc_uint32 i = 0;
  while (i < string_size)
  {
    sc_dictionary_node * next = _sc_dictionary_get_next_node(dictionary, node, string[i]);
    if (SC_DICTIONARY_NODE_IS_VALID(next) &&
        (sc_str_has_prefix(string + i, next->offset) || sc_str_cmp(string + i, next->offset)))
    {
      node = next;
      i += node->offset_size;
    }
    else
      break;
  }

  // check suffixes matching
  if (i == string_size &&
      (node->offset == null_ptr || sc_str_cmp(node->offset, string + (string_size - node->offset_size))))
  {
    return node;
  }

  return null_ptr;
}

sc_bool sc_dictionary_has(const sc_dictionary * dictionary, const sc_char * string, sc_uint32 string_size)
{
  const sc_dictionary_node * last =
      sc_dictionary_get_last_node_from_node(dictionary, dictionary->root, string, string_size);

  return SC_DICTIONARY_NODE_IS_VALID(last);
}

void * sc_dictionary_get_by_key(const sc_dictionary * dictionary, const sc_char * string, const sc_uint32 string_size)
{
  const sc_dictionary_node * last =
      sc_dictionary_get_last_node_from_node(dictionary, dictionary->root, string, string_size);

  if (SC_DICTIONARY_NODE_IS_VALID(last))
    return last->data;

  return null_ptr;
}

sc_bool sc_dictionary_get_by_key_prefix(
    const sc_dictionary * dictionary,
    const sc_char * string,
    const sc_uint32 string_size,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_dictionary_node * node = dictionary->root;

  sc_uint32 i = 0;
  while (i < string_size)
  {
    sc_dictionary_node * next = _sc_dictionary_get_next_node(dictionary, node, string[i]);
    if (SC_DICTIONARY_NODE_IS_VALID(next) &&
        (sc_str_has_prefix(string + i, next->offset) || sc_str_cmp(string + i, next->offset)))
    {
      node = next;
      i += node->offset_size;
    }
    else
      break;
  }

  if (i == string_size)
    callable(node, dest);

  for (sc_uint8 num = 0; num < dictionary->size; ++num)
  {
    sc_uint64 const bucket_idx = SC_DICTIONARY_GET_BUCKET_NUM(num);
    sc_dictionary_node ** next_bucket = node->next[bucket_idx];
    if (SC_DICTIONARY_NODE_IS_NOT_VALID(next_bucket))
      continue;

    sc_uint64 const child_idx = SC_DICTIONARY_GET_CHILD_NUM(num);
    sc_dictionary_node * next = next_bucket[child_idx];
    if (SC_DICTIONARY_NODE_IS_NOT_VALID(next))
      continue;

    if (i == string_size || sc_str_has_prefix(next->offset, string + i))
    {
      if (!callable(next, dest))
        return SC_FALSE;

      if (!sc_dictionary_visit_down_node_from_node(dictionary, next, callable, dest))
        return SC_FALSE;
    }
  }
  return SC_TRUE;
}

sc_bool sc_dictionary_visit_down_node_from_node(
    sc_dictionary const * dictionary,
    sc_dictionary_node * node,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  for (sc_uint8 num = 0; num < dictionary->size; ++num)
  {
    sc_uint64 const bucket_idx = SC_DICTIONARY_GET_BUCKET_NUM(num);
    sc_dictionary_node ** next_bucket = node->next[bucket_idx];
    if (SC_DICTIONARY_NODE_IS_NOT_VALID(next_bucket))
      continue;

    sc_uint64 const child_idx = SC_DICTIONARY_GET_CHILD_NUM(num);
    sc_dictionary_node * next = next_bucket[child_idx];
    if (SC_DICTIONARY_NODE_IS_NOT_VALID(next))
      continue;

    if (!callable(next, dest))
      return SC_FALSE;

    if (!sc_dictionary_visit_down_node_from_node(dictionary, next, callable, dest))
      return SC_FALSE;
  }

  return SC_TRUE;
}

sc_bool sc_dictionary_visit_down_nodes(
    sc_dictionary const * dictionary,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  return sc_dictionary_visit_down_node_from_node(dictionary, dictionary->root, callable, dest);
}

sc_bool sc_dictionary_visit_up_node_from_node(
    sc_dictionary const * dictionary,
    sc_dictionary_node * node,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  for (sc_uint8 num = 0; num < dictionary->size; ++num)
  {
    sc_uint64 const bucket_idx = SC_DICTIONARY_GET_BUCKET_NUM(num);
    sc_dictionary_node ** next_bucket = node->next[bucket_idx];
    if (SC_DICTIONARY_NODE_IS_NOT_VALID(next_bucket))
      continue;

    sc_uint64 const child_idx = SC_DICTIONARY_GET_CHILD_NUM(num);
    sc_dictionary_node * next = next_bucket[child_idx];
    if (SC_DICTIONARY_NODE_IS_NOT_VALID(next))
      continue;

    if (!sc_dictionary_visit_up_node_from_node(dictionary, next, callable, dest))
      return SC_FALSE;

    if (!callable(next, dest))
      return SC_FALSE;
  }

  return SC_TRUE;
}

sc_bool sc_dictionary_visit_up_nodes(
    sc_dictionary const * dictionary,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  return sc_dictionary_visit_up_node_from_node(dictionary, dictionary->root, callable, dest);
}
