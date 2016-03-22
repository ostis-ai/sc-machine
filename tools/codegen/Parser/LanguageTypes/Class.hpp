#pragma once

#include "LanguageType.hpp"

#include "Constructor.hpp"
#include "Field.hpp"
#include "Global.hpp"
#include "Method.hpp"
#include "Function.hpp"

struct BaseClass
{
    BaseClass(const Cursor &cursor);

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

    void GenerateCodeInit(std::stringstream & outCode) const;
    void GenerateCodeStaticInit(std::stringstream & outCode) const;

    std::string GetGeneratedBodyLine() const;

protected:
    void GenerateFieldsInitCode(std::stringstream & outCode) const;
    void GenerateStaticFieldsInitCode(std::stringstream & outCode) const;
    

private:
    std::string m_name;
    std::string m_displayName;
    std::string m_qualifiedName;

    std::vector<BaseClass*> m_baseClasses;
        
    std::vector<Constructor*> m_constructors;

    typedef std::vector<Field*> tFieldsVector;
    tFieldsVector m_fields;
    
    typedef std::vector<Global*> tStaticFieldsVector;
    tStaticFieldsVector m_staticFields;
    
    typedef std::vector<Method*> tMethodVector;
    tMethodVector m_methods;

    std::vector<Function*> m_staticMethods;

    bool isScObject;
};
