#pragma once

#include <clang-c/Index.h>

#include <string>

class Cursor;

class CursorType
{
public:
  CursorType(CXType const & handle);

  std::string GetDisplayName(void) const;

  int GetArgumentCount(void) const;
  CursorType GetArgument(unsigned index) const;

  CursorType GetCanonicalType(void) const;

  Cursor GetDeclaration(void) const;

  CXTypeKind GetKind(void) const;

  bool IsConst(void) const;

private:
  CXType m_handle;
};
