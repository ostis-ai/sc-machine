#pragma once

#include "LanguageType.hpp"

#include "Constructor.hpp"
#include "Field.hpp"
#include "Function.hpp"
#include "Global.hpp"
#include "Method.hpp"

#include <memory>
#include <vector>

struct BaseClass
{
  BaseClass(const Cursor & cursor);

  bool IsNative() const;

  std::string name;
};

class Class : public LanguageType
{
  // to access m_qualifiedName
  friend class Global;
  friend class Function;
  friend class Method;
  friend class Constructor;
  friend class Field;

public:
  Class(const Cursor & cursor, const Namespace & currentNamespace);

  bool ShouldGenerate(void) const;
  bool IsAgent() const;
  bool IsActionAgent() const;
  bool IsModule() const;

  void GenerateCode(std::string const & fileId, std::stringstream & outCode, ReflectionParser * parser) const;
  void GenerateCodeInit(std::stringstream & outCode) const;
  void GenerateCodeStaticInit(std::stringstream & outCode) const;

  std::string GetGeneratedBodyLine() const;
  std::string GetQualifiedName() const;

protected:
  void GenerateFieldsInitCode(std::stringstream & outCode) const;
  void GenerateStaticFieldsInitCode(std::stringstream & outCode) const;
  void GenerateDeclarations(std::stringstream & outCode) const;
  void GenerateImpl(std::stringstream & outCode) const;

  BaseClass const * GetBaseClass(std::string const & name) const;
  BaseClass const * GetBaseAgentClass() const;

private:
  std::string m_name;
  std::string m_displayName;
  std::string m_qualifiedName;

  using BaseClassVector = std::vector<std::shared_ptr<BaseClass>>;
  using ConstructorVector = std::vector<std::shared_ptr<Constructor>>;
  using FieldVector = std::vector<std::shared_ptr<Field>>;
  using StaticFieldVector = std::vector<std::shared_ptr<Global>>;
  using MethodVector = std::vector<std::shared_ptr<Method>>;
  using StaticMethodVector = std::vector<std::shared_ptr<Function>>;

  BaseClassVector m_baseClasses;
  ConstructorVector m_constructors;
  FieldVector m_fields;
  StaticFieldVector m_staticFields;
  MethodVector m_methods;
  StaticMethodVector m_staticMethods;

  bool m_isScObject;

  mutable ReflectionParser * m_parser;
};
