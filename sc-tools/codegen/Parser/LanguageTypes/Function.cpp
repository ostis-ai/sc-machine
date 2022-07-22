#include "Cursor.hpp"

#include "LanguageTypes/Function.hpp"
#include "LanguageTypes/Class.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

Function::Function(Cursor const & cursor, Namespace const & currentNamespace)
  : LanguageType(cursor, currentNamespace)
  , Invokable(cursor)
  , m_name(cursor.GetSpelling())
{
  m_metaData.Check();
}