#include "Precompiled.hpp"

#include "LanguageTypes/Class.hpp"
#include "LanguageTypes/Method.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

Method::Method(const Cursor & cursor, const Namespace & currentNamespace, Class * parent)
    : LanguageType(cursor, currentNamespace)
    , Invokable(cursor)
    , m_parent(parent)
    , m_name(cursor.GetSpelling())
{
	m_metaData.Check();
}

Method::~Method()
{

}
