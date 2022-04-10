/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <glib.h>
#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"

extern "C"
{
#include "sc-core/sc-store/sc-container/sc_struct_node.h"
#include "sc-core/sc-store/sc-container/sc-list/sc_list.h"
#include "sc-core/sc-store/sc-container/sc-iterator/sc_container_iterator.h"
}

sc_bool scalar_compare(void * value, void * other)
{
  return *(sc_uint8 *)value == *(sc_uint8 *)other;
}

TEST_CASE("sc-list", "[test sc-list]")
{
  sc_list * list;
  sc_list_init(&list);

  sc_uint8 size = 10;
  sc_uint8 * values = g_new(sc_uint8, size);
  sc_uint8 i = 0;
  for (; i < size; ++i)
  {
    values[i] = i;
    sc_list_push_back(list, (void *)(&values[i]), 1);
  }

  sc_uint8 v = 5;
  g_assert(sc_list_remove_if(list, (void *)(&v), 1, scalar_compare));

  sc_struct_node * last;
  i = list->size;
  for (; i >= 0 && i <= list->size; --i)
  {
    last = sc_list_pop_back(list);
    sc_uint8 value = *(sc_uint8 *)last->data->value;

    if (i == 5)
      --i;

    g_assert(value == i);
  }

  g_assert(sc_list_pop_back(list) == nullptr);
  g_free(values);
  sc_list_destroy(list);
}

TEST_CASE("sc-list-iterator", "[test sc-list-iterator]")
{
  sc_list * list;
  sc_list_init(&list);

  sc_uint8 size = 10;
  sc_uint8 * values = g_new(sc_uint8, size);
  sc_uint8 i = 0;
  for (; i < size; ++i)
  {
    values[i] = i;
    sc_list_push_back(list, (void *)(&values[i]), 1);
  }

  i = 0;
  sc_struct_node_data * data;
  sc_iterator * it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    data = sc_iterator_get(it);
    sc_uint8 value = *(sc_uint8 *)data->value;

    g_assert(value == i);
    ++i;
  }
  g_assert(i == size);

  i = size - 1;
  sc_iterator_reset(it);
  while (sc_iterator_prev(it))
  {
    data = sc_iterator_get(it);
    sc_uint8 value = *(sc_uint8 *)data->value;

    g_assert(value == i);
    --i;
  }
  g_assert(i == 255);

  g_free(values);
  sc_iterator_destroy(it);
  sc_list_destroy(list);
}
