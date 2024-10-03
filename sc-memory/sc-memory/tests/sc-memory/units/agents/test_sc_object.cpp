/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_object.hpp>

using ScObjectTest = ScMemoryTest;

namespace spaceA
{
class ClassA
{
};
}  // namespace spaceA

class ClassB
{
};

TEST_F(ScObjectTest, Copy)
{
  ScObject object1;
  ScObject object2 = ScObject();

  object1 = object2;
  EXPECT_NE(&object1, &object2);
}

TEST_F(ScObjectTest, GetName)
{
  EXPECT_EQ(ScObject::GetName<spaceA::ClassA>(), "spaceA::ClassA");
  EXPECT_EQ(ScObject::GetName<ClassB>(), "ClassB");
}
