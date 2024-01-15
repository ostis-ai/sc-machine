#include <gtest/gtest.h>

extern "C"
{
#include "sc-core/sc-store/sc-base/sc_allocator.h"
#include "sc-core/sc-store/sc-container/sc-pair/sc_pair.h"
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
