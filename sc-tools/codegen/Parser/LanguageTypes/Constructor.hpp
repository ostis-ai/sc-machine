#pragma once

#include "LanguageType.hpp"
#include "Invokable.hpp"

class Constructor final
  : public LanguageType
  , public Invokable
{
public:
  Constructor(Cursor const & cursor, Namespace const & currentNamespace, class Class * parent);

  bool ShouldCompile() const;

private:
  bool isAccessible() const;
  std::string getTemplateParameters() const;

private:
  class Class * m_parent;
};
