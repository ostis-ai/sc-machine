#include "LanguageTypes/Method.hpp"

#include "Cursor.hpp"
#include "LanguageTypes/Class.hpp"

Method::Method(Cursor const & cursor, Namespace const & currentNamespace)
  : LanguageType(cursor, currentNamespace)
  , Invokable(cursor)
  , m_name(cursor.GetSpelling())
{
  m_metaData.Check();
}
