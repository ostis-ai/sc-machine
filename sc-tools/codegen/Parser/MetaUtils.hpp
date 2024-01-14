#pragma once

#include "CursorType.hpp"
#include "Types.hpp"

class Cursor;

namespace utils
{
void ToString(CXString const & str, std::string & output);

std::string GetQualifiedName(std::string const & displayName, Namespace const & currentNamespace);

std::string GetQualifiedName(Cursor const & cursor, Namespace const & currentNamespace);

void FatalError(std::string const & error);
}  // namespace utils
