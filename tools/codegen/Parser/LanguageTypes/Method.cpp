#include "Precompiled.hpp"

#include "LanguageTypes/Class.hpp"
#include "LanguageTypes/Method.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

Method::Method(
    const Cursor &cursor, 
    const Namespace &currentNamespace, 
    Class *parent
)
    : LanguageType( cursor, currentNamespace )
    , Invokable( cursor )
    , m_isConst( cursor.IsConst( ) )
    , m_parent( parent )
    , m_name( cursor.GetSpelling( ) )
{
    
}

bool Method::ShouldCompile(void) const
{
    return isAccessible( );
}

bool Method::isAccessible(void) const
{
    return m_accessModifier == CX_CXXPublic && 
           !m_metaData.GetFlag( kMetaDisable );
}

std::string Method::getQualifiedSignature(void) const
{
    auto argsList = boost::algorithm::join( m_signature, ", " );

    std::string constNess = m_isConst ? " const" : "";

    return (boost::format( "%1%(%2%::*)(%3%)%4%" ) % 
        m_returnType % 
        m_parent->m_qualifiedName % 
        argsList % constNess
    ).str( );
}