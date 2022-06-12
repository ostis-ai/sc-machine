#pragma once

#include "CursorType.hpp"
#include "Types.hpp"

class Cursor;

namespace utils
{
void ToString(const CXString & str, std::string & output);

std::string GetQualifiedName(std::string const & displayName, Namespace const & currentNamespace);

std::string GetQualifiedName(Cursor const & cursor, Namespace const & currentNamespace);

void FatalError(const std::string & error);
}  // namespace utils
