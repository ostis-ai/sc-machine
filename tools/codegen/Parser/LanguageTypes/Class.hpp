#pragma once

#include "LanguageType.hpp"


struct BaseClass
{
    BaseClass(const Cursor &cursor);

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
    Class(const Cursor &cursor, const Namespace &currentNamespace);
    virtual ~Class(void);

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

	typedef std::vector<BaseClass*> tBaseClassVector;
	tBaseClassVector m_baseClasses;
        
    std::vector<class Constructor*> m_constructors;

    typedef std::vector<class Field*> tFieldsVector;
    tFieldsVector m_fields;
    
    typedef std::vector<class Global*> tStaticFieldsVector;
    tStaticFieldsVector m_staticFields;
    
    typedef std::vector<class Method*> tMethodVector;
    tMethodVector m_methods;

    std::vector<class Function*> m_staticMethods;

    bool m_isScObject;

	mutable ReflectionParser * m_parser;
};
