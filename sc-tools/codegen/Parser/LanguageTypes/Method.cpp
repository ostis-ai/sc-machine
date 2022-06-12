#include "LanguageTypes/Class.hpp"
#include "LanguageTypes/Method.hpp"

#include "Cursor.hpp"

Method::Method(Cursor const & cursor, Namespace const & currentNamespace)
  : LanguageType(cursor, currentNamespace)
  , Invokable(cursor)
  , m_name(cursor.GetSpelling())
{
  m_metaData.Check();
}
