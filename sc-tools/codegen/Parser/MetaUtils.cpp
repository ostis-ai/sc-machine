#include "MetaUtils.hpp"

#include "Cursor.hpp"
#include "CursorType.hpp"

#include <iostream>

#include <boost/algorithm/string/join.hpp>

namespace utils
{
void ToString(CXString const & str, std::string & output)
{
  auto cstr = clang_getCString(str);

  output = cstr;

  clang_disposeString(str);
}

std::string GetQualifiedName(std::string const & displayName, Namespace const & currentNamespace)
{
  auto name = boost::algorithm::join(currentNamespace, "::");

  if (!currentNamespace.empty())
    name += "::";

  name += displayName;

  return name;
}

std::string GetQualifiedName(Cursor const & cursor, Namespace const & currentNamespace)
{
  return GetQualifiedName(cursor.GetSpelling(), currentNamespace);
}

void FatalError(std::string const & error)
{
  std::cerr << "Error: " << error << std::endl;

  exit(EXIT_FAILURE);
}

}  // namespace utils
