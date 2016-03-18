#include "Precompiled.hpp"

#include "LanguageTypes/Function.hpp"
#include "LanguageTypes/Class.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

Function::Function(
    const Cursor &cursor, 
    const Namespace &currentNamespace, 
    Class *parent
)
    : LanguageType( cursor, currentNamespace )
    , Invokable( cursor )
    , m_parent( parent )
    , m_name( cursor.GetSpelling( ) )
    , m_qualifiedName( utils::GetQualifiedName( cursor, currentNamespace ) )
{
        
}

bool Function::ShouldCompile(void) const
{
    return isAccessible( );
}

bool Function::isAccessible(void) const
{
    if (m_parent && m_accessModifier != CX_CXXPublic)
        return false;

    return m_enabled;
}

std::string Function::getQualifiedSignature(void) const
{
    auto argsList = boost::algorithm::join( m_signature, ", " );

    return (boost::format( "%1%(*)(%2%)" ) % m_returnType % argsList).str( );
}