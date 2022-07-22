#include "Cursor.hpp"

#include "MetaDataConfig.hpp"
#include "MetaUtils.hpp"

#include "LanguageTypes/Global.hpp"
#include "LanguageTypes/Class.hpp"
#include "LanguageTypes/Field.hpp"

Global::Global(Cursor const & cursor, Namespace const & currentNamespace, Class * parent)
  : LanguageType(cursor, currentNamespace)
  , m_isConst(cursor.GetType().IsConst())
  , m_hasExplicitGetter(m_metaData.GetFlag(kMetaExplicitGetter))
  , m_hasExplicitSetter(m_metaData.GetFlag(kMetaExplicitSetter))
  , m_parent(parent)
  , m_name(cursor.GetSpelling())
  , m_qualifiedName(utils::GetQualifiedName(cursor, currentNamespace))
  , m_type(cursor.GetType().GetDisplayName())
{
  auto displayName = m_metaData.GetNativeString(kMetaDisplayName);
  m_displayName = displayName.empty() ? m_qualifiedName : utils::GetQualifiedName(displayName, currentNamespace);

  m_metaData.Check();
}

bool Global::ShouldCompile() const
{
  return isAccessible();
}

void Global::GenerateInitCode(std::stringstream & outCode) const
{
  /// TODO: merge with field code generation
  if (m_metaData.HasProperty(Props::Keynode))
  {
    Field::GenerateResolveKeynodeCode(
        m_metaData.GetNativeString(Props::Keynode), m_displayName, Field::GetForceType(m_metaData), outCode);
  }
  else if (m_metaData.HasProperty(Props::Template))
  {
    Field::GenerateTemplateBuildCode(m_metaData.GetNativeString(Props::Template), m_displayName, outCode);
  }
}

bool Global::isAccessible() const
{
  return isGetterAccessible() || isSetterAccessible();
}

bool Global::isGetterAccessible() const
{
  if (m_enabled)
  {
    if (m_parent)
      return m_hasExplicitGetter || m_accessModifier == CX_CXXPublic;

    return true;
  }

  return false;
}

bool Global::isSetterAccessible() const
{
  if (m_isConst)
    return false;

  if (m_enabled)
  {
    if (m_parent)
      return m_hasExplicitSetter || m_accessModifier == CX_CXXPublic;

    return true;
  }

  return false;
}
