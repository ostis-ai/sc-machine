#pragma once

#include "LanguageType.hpp"

class Class;

class Field final : public LanguageType
{
public:
  Field(Cursor const & cursor, Namespace const & currentNamespace);

  bool ShouldCompile() const;

  void GenarateInitCode(std::stringstream & outCode) const;

  static std::string GetForceType(MetaDataManager const & metaData);

  static void GenerateResolveKeynodeCode(
      std::string const & sysIdtf,
      std::string const & displayName,
      std::string const & forceType,
      std::stringstream & outCode);
  static void GenerateTemplateBuildCode(
      std::string const & sysIdtf,
      std::string const & displayName,
      std::stringstream & outCode);

  std::string const & GetDisplayName() const;

private:
  bool isAccessible() const;
  bool isGetterAccessible() const;
  bool isSetterAccessible() const;

private:
  bool m_isConst;
  bool m_hasExplicitGetter;
  bool m_hasExplicitSetter;

  std::string m_name;
  std::string m_displayName;
  std::string m_type;
};
