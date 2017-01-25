# Carete unit test

To append new unit test, you should create file `sc-memory/test/wrap/units/test_my.cpp` with base content

```cpp
/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"

UNIT_TEST(my_test_name)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "my_test_name");

  /* There you should implement your test logic.
   * Use SC_CHECK_.. functions for check test results
   */

   SUBTEST_START(my_subtest_name)
   {
     /* There you can implement simple subtest.
      * For example check the same data, but with some another logic.
      */
   }
   SUBTEST_END()

   SUBTEST_START(my_subtest_name_2)
   {
     // You can implement as many as you want subtests
   }
   SUBTEST_END()
}

UNIT_TEST(me_test_name_2)
{
  // You can implement as many as you want tests
}
```

For example you can see any test in `sc-memory/test/wrap/units/` folder.
