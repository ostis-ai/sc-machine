#include "CursorType.hpp"
#include "Cursor.hpp"

#include "MetaUtils.hpp"

CursorType::CursorType(CXType const & handle)
  : m_handle(handle)
{
}

std::string CursorType::GetDisplayName(void) const
{
  std::string displayName;

  utils::ToString(clang_getTypeSpelling(m_handle), displayName);

  return displayName;
}

int CursorType::GetArgumentCount(void) const
{
  return clang_getNumArgTypes(m_handle);
}

CursorType CursorType::GetArgument(unsigned index) const
{
  return clang_getArgType(m_handle, index);
}

CursorType CursorType::GetCanonicalType(void) const
{
  return clang_getCanonicalType(m_handle);
}

Cursor CursorType::GetDeclaration(void) const
{
  return clang_getTypeDeclaration(m_handle);
}

CXTypeKind CursorType::GetKind(void) const
{
  return m_handle.kind;
}

bool CursorType::IsConst(void) const
{
  return clang_isConstQualifiedType(m_handle) ? true : false;
}
