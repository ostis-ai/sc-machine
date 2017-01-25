#pragma once

#include "LanguageType.hpp"

class Class;

class Global : public LanguageType
{
public:
  Global(
      const Cursor &cursor,
      const Namespace &currentNamespace,
      Class *parent = nullptr
      );

  virtual ~Global(void) { }

  bool ShouldCompile(void) const;
  void GenerateInitCode(std::stringstream & outCode) const;


private:
  bool m_isConst;

  bool m_hasExplicitGetter;
  bool m_hasExplicitSetter;

  Class *m_parent;

  std::string m_name;
  std::string m_displayName;
  std::string m_qualifiedName;
  std::string m_type;

  bool isAccessible(void) const;
  bool isGetterAccessible(void) const;
  bool isSetterAccessible(void) const;
};
