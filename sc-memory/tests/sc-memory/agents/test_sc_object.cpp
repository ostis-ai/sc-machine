#include "sc_test.hpp"

#include "sc-memory/sc_object.hpp"

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

TEST_F(ScObjectTest, GetName)
{
  EXPECT_EQ(ScObject::GetName<spaceA::ClassA>(), "spaceA::ClassA");
  EXPECT_EQ(ScObject::GetName<ClassB>(), "ClassB");
}
