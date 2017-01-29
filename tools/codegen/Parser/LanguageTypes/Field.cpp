#include "Cursor.hpp"

#include "MetaDataConfig.hpp"

#include "LanguageTypes/Class.hpp"
#include "LanguageTypes/Field.hpp"

Field::Field(
    const Cursor &cursor,
    const Namespace &currentNamespace,
    Class *parent)
  : LanguageType(cursor, currentNamespace)
  , m_isConst(cursor.GetType().IsConst())
  , m_parent(parent)
  , m_name(cursor.GetSpelling())
  , m_type(cursor.GetType().GetDisplayName())
{
  auto displayName = m_metaData.GetNativeString(kMetaDisplayName);

  if (displayName.empty())
    m_displayName = m_name;
  else
    m_displayName = displayName;

  m_metaData.Check();
}

bool Field::ShouldCompile(void) const
{
  return isAccessible();
}

bool Field::isAccessible(void) const
{
  return (m_hasExplicitGetter || m_hasExplicitSetter) ||
      (
        m_accessModifier == CX_CXXPublic &&
        !m_metaData.GetFlag(kMetaDisable)
        );
}

bool Field::isGetterAccessible(void) const
{
  return m_hasExplicitGetter || m_accessModifier == CX_CXXPublic;
}

bool Field::isSetterAccessible(void) const
{
  return m_hasExplicitSetter ||
      (!m_isConst && m_accessModifier == CX_CXXPublic);
}

std::string Field::GetForceType(MetaDataManager const & metaData)
{
  if (metaData.HasProperty(Props::ForceCreate))
  {
    std::string const value = metaData.GetNativeString(Props::ForceCreate);
    return value.empty() ? "ScType::Node" : value;
  }

  return "";
}

void Field::GenarateInitCode(std::stringstream & outCode) const
{
  if (m_metaData.HasProperty(Props::Keynode))
  {
    GenerateResolveKeynodeCode(m_metaData.GetNativeString(Props::Keynode),
                               m_displayName,
                               GetForceType(m_metaData),
                               outCode);
  }
  else if (m_metaData.HasProperty(Props::Template))
  {
    GenerateTemplateBuildCode(m_metaData.GetNativeString(Props::Template),
                              m_displayName, outCode);
  }
}

void Field::GenerateTemplateBuildCode(std::string const & sysIdtf, std::string const & displayName, std::stringstream & outCode)
{
  std::string vName = displayName + "_Addr_";
  outCode << "ScAddr " << vName << "; ";
  GenerateResolveKeynodeCode(sysIdtf, vName, "", outCode);
  outCode << " if (result) { result = result && ctx.HelperBuildTemplate(" << displayName << ", " << vName << "); }";
}

void Field::GenerateResolveKeynodeCode(
  std::string const & sysIdtf, 
  std::string const & displayName,
  std::string const & forceCreation,
  std::stringstream & outCode)
{
  outCode << "  result = result && ctx.HelperResolveSystemIdtf(\""
    << sysIdtf << "\", "
    << displayName;
  
  if (!forceCreation.empty())
  {
    outCode << ", " << forceCreation;
  }         

  outCode << ");";
}

std::string const & Field::GetDisplayName() const
{
  return m_displayName;
}
