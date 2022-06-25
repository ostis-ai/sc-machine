#include <gtest/gtest.h>

extern "C"
{
#include "sc-core/sc-store/sc-container/sc_struct_node.h"
#include "sc-core/sc-store/sc-container/sc-list/sc_list.h"
#include "sc-core/sc-store/sc-container/sc-iterator/sc_container_iterator.h"
}

#include "sc_test.hpp"

sc_bool scalar_compare(void * value, void * other)
{
  return *(sc_uint8 *)value == *(sc_uint8 *)other;
}

using ScListTest = ScMemoryTest;

TEST_F(ScListTest, sc_list)
{
  sc_list * list;
  sc_list_init(&list);

  sc_uint8 size = 10;
  auto * values = (sc_uint8 *)malloc(sizeof(sc_uint8) * size);
  sc_uint8 i = 0;
  for (; i < size; ++i)
  {
    values[i] = i;
    sc_list_push_back(list, (void *)(&values[i]));
  }

  sc_uint8 v = 5;
  EXPECT_TRUE(sc_list_remove_if(list, (void *)(&v), scalar_compare));

  sc_struct_node * last;
  i = list->size;
  for (; i >= 0 && i <= list->size; --i)
  {
    last = sc_list_pop_back(list);
    sc_uint8 value = *(sc_uint8 *)last->data;

    if (i == 5)
      --i;

    EXPECT_TRUE(value == i);
  }

  EXPECT_TRUE(sc_list_pop_back(list) == nullptr);
  free(values);
  sc_list_destroy(list);
}

TEST_F(ScListTest, sc_list_iterator)
{
  sc_list * list;
  sc_list_init(&list);

  sc_uint8 size = 10;
  auto * values = (sc_uint8 *)malloc(sizeof(sc_uint8) * size);
  sc_uint8 i = 0;
  for (; i < size; ++i)
  {
    values[i] = i;
    sc_list_push_back(list, (void *)(&values[i]));
  }

  i = 0;
  void * data;
  sc_iterator * it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    data = sc_iterator_get(it);
    sc_uint8 value = *(sc_uint8 *)data;

    EXPECT_TRUE(value == i);
    ++i;
  }
  EXPECT_TRUE(i == size);

  i = size - 1;
  sc_iterator_reset(it);
  while (sc_iterator_prev(it))
  {
    data = sc_iterator_get(it);
    sc_uint8 value = *(sc_uint8 *)data;

    EXPECT_TRUE(value == i);
    --i;
  }
  EXPECT_TRUE(i == 255);

  free(values);
  sc_iterator_destroy(it);
  sc_list_destroy(list);
}

