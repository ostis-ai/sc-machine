/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

extern "C"
{
#include <sc-core/sc-base/sc_allocator.h>
#include <sc-core/sc-container/sc_list.h>
#include <sc-core/sc-container/sc_container_iterator.h>

#include <sc-store/sc-container/sc_struct_node.h>
}

sc_bool scalar_compare(void * value, void * other)
{
  return *(sc_uint8 *)value == *(sc_uint8 *)other;
}

TEST(ScListTest, sc_list)
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
  for (i = list->size; i <= list->size; --i)
  {
    last = sc_list_pop_back(list);
    sc_uint8 value = *(sc_uint8 *)last->data;

    if (i == 5)
      --i;

    EXPECT_TRUE(value == i);
  }

  EXPECT_TRUE(sc_list_pop_back(list) == nullptr);
  sc_mem_free(values);
  sc_list_destroy(list);
}

TEST(ScListTest, sc_list_null_ptr)
{
  EXPECT_FALSE(sc_list_init(nullptr));
  EXPECT_FALSE(sc_list_destroy(nullptr));
  EXPECT_FALSE(sc_list_clear(nullptr));
  EXPECT_EQ(sc_list_push(nullptr, nullptr, nullptr), nullptr);
  EXPECT_EQ(sc_list_push_back(nullptr, nullptr), nullptr);
  EXPECT_EQ(sc_list_pop_back(nullptr), nullptr);
  EXPECT_FALSE(sc_list_remove_if(nullptr, nullptr, nullptr));
  EXPECT_FALSE(sc_list_remove_if(nullptr, nullptr, nullptr));
  EXPECT_EQ(sc_list_front(nullptr), nullptr);
  EXPECT_EQ(sc_list_back(nullptr), nullptr);
}

TEST(ScListTest, sc_list_iterator)
{
  sc_list * list;
  sc_list_init(&list);
  EXPECT_EQ(sc_list_front(list), nullptr);
  EXPECT_EQ(sc_list_back(list), nullptr);

  sc_uint8 size = 10;
  auto * values = (sc_uint8 *)malloc(sizeof(sc_uint8) * size);
  sc_uint8 i = 0;
  for (; i < size; ++i)
  {
    values[i] = i;
    sc_list_push_back(list, (void *)(&values[i]));
  }

  EXPECT_NE(sc_list_front(list), nullptr);
  EXPECT_NE(sc_list_back(list), nullptr);

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

  sc_mem_free(values);
  sc_iterator_destroy(it);
  sc_list_destroy(list);
}
