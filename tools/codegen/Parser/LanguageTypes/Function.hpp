#pragma once

#include "LanguageType.hpp"
#include "Invokable.hpp"

class Function final
  : public LanguageType
  , public Invokable
{
public:
  Function(Cursor const & cursor, Namespace const & currentNamespace);

private:
  std::string m_name;
};
