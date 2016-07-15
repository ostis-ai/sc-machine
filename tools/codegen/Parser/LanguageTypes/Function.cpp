#include "Precompiled.hpp"

#include "LanguageTypes/Function.hpp"
#include "LanguageTypes/Class.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

Function::Function(const Cursor & cursor, const Namespace & currentNamespace, Class * parent)
    : LanguageType(cursor, currentNamespace)
    , Invokable(cursor)
    , m_parent(parent)
    , m_name(cursor.GetSpelling())
{
	m_metaData.Check();
}

Function::~Function()
{

}
