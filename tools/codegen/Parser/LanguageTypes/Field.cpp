#include "Precompiled.hpp"

#include "LanguageTypes/Class.hpp"
#include "LanguageTypes/Field.hpp"

Field::Field(
    const Cursor &cursor, 
    const Namespace &currentNamespace, 
    Class *parent
)
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

    m_explicitGetter = m_metaData.GetNativeString(kMetaExplicitGetter);
    m_hasExplicitGetter = !m_explicitGetter.empty();

    m_explicitSetter = m_metaData.GetNativeString(kMetaExplicitSetter);
    m_hasExplicitSetter = !m_explicitSetter.empty();
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

void Field::GenarateInitCode(std::stringstream & outCode) const
{
    outCode << "result = result && ctx.helperFindBySystemIdtf(\"" << m_metaData.GetNativeString("SysIdtf") << "\", " << m_displayName << ");";
}

std::string const & Field::GetDisplayName() const
{
    return m_displayName;
}