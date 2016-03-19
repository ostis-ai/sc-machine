#include "Precompiled.hpp"

#include "Class.hpp"
#include "Constructor.hpp"

#include <boost/algorithm/string/join.hpp>

Constructor::Constructor(
    const Cursor &cursor, 
    const Namespace &currentNamespace, 
    Class *parent
)
    : LanguageType(cursor, currentNamespace)
    , Invokable(cursor)
    , m_parent(parent)
{
        
}

bool Constructor::ShouldCompile(void) const
{
    return isAccessible();
}

bool Constructor::isAccessible(void) const
{
    return m_accessModifier == CX_CXXPublic && 
           !m_metaData.GetFlag(kMetaDisable);
}

std::string Constructor::getTemplateParameters(void) const
{
    auto params(m_signature);

    params.insert(params.begin(), m_parent->m_qualifiedName);

    // parent type, arg types, ...
    return boost::algorithm::join(params, ", ");
}