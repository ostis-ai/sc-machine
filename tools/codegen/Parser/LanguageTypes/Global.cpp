#include "Precompiled.hpp"

#include "LanguageTypes/Global.hpp"
#include "LanguageTypes/Class.hpp"

Global::Global(
    const Cursor &cursor, 
    const Namespace &currentNamespace, 
    Class *parent
)
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

    if (displayName.empty())
    {
        m_displayName = m_qualifiedName;
    }
    else
    {
        m_displayName = 
            utils::GetQualifiedName(displayName, currentNamespace);
    }
}

bool Global::ShouldCompile(void) const
{
    return isAccessible();
}

bool Global::isAccessible(void) const
{
    return isGetterAccessible() || isSetterAccessible();
}

bool Global::isGetterAccessible(void) const
{
    if (m_enabled)
    {
        if (m_parent)
            return m_hasExplicitGetter || m_accessModifier == CX_CXXPublic;

        return true;
    }

    return false;
}

bool Global::isSetterAccessible(void) const
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