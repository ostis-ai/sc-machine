#include "LanguageType.hpp"

#include "MetaDataConfig.hpp"
#include "Cursor.hpp"

LanguageType::LanguageType(Cursor const & cursor, Namespace const & currentNamespace)
  : m_metaData(cursor)
  , m_enabled(m_metaData.GetFlag(kMetaEnable))
  , m_ptrTypeEnabled(!m_metaData.GetFlag(kMetaDisablePtrType))
  , m_constPtrTypeEnabled(!m_metaData.GetFlag(kMetaDisableConstPtrType))
  , m_currentNamespace(currentNamespace)
  , m_accessModifier(cursor.GetAccessModifier())
{
}

MetaDataManager const & LanguageType::GetMetaData() const
{
  return m_metaData;
}
