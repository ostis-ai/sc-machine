/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

extern "C"
{
#include <sc-core/sc-base/sc_allocator.h>
#include <sc-store/sc-container/sc_pair.h>
}

TEST(ScPairTest, sc_pair)
{
  sc_pair * pair;
  sc_pair_initialize(&pair, (void *)1, (void *)2);
  EXPECT_EQ((sc_uint64)pair->first, 1u);
  EXPECT_EQ((sc_uint64)pair->second, 2u);
  sc_mem_free(pair);

  pair = sc_make_pair((void *)1, (void *)2);
  EXPECT_EQ((sc_uint64)pair->first, 1u);
  EXPECT_EQ((sc_uint64)pair->second, 2u);
  sc_mem_free(pair);
}
