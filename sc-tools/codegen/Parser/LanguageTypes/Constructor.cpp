#include "Class.hpp"
#include "Constructor.hpp"
#include "MetaDataConfig.hpp"

#include <boost/algorithm/string/join.hpp>

Constructor::Constructor(Cursor const & cursor, Namespace const & currentNamespace, Class * parent)
  : LanguageType(cursor, currentNamespace)
  , Invokable(cursor)
  , m_parent(parent)
{
}

bool Constructor::ShouldCompile() const
{
  return isAccessible();
}

bool Constructor::isAccessible() const
{
  return m_accessModifier == CX_CXXPublic && !m_metaData.GetFlag(kMetaDisable);
}

std::string Constructor::getTemplateParameters() const
{
  auto params(m_signature);

  params.insert(params.begin(), m_parent->m_qualifiedName);

  // parent type, arg types, ...
  return boost::algorithm::join(params, ", ");
}
