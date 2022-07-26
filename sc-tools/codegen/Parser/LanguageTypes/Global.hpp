#pragma once

#include "LanguageType.hpp"

class Class;

class Global final : public LanguageType
{
public:
  Global(Cursor const & cursor, Namespace const & currentNamespace, class Class * parent = nullptr);

  bool ShouldCompile() const;
  void GenerateInitCode(std::stringstream & outCode) const;

private:
  bool isAccessible() const;
  bool isGetterAccessible() const;
  bool isSetterAccessible() const;

private:
  bool m_isConst;
  bool m_hasExplicitGetter;
  bool m_hasExplicitSetter;

  class Class * m_parent;

  std::string m_name;
  std::string m_displayName;
  std::string m_qualifiedName;
  std::string m_type;
};
