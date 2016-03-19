#pragma once

#include "LanguageType.hpp"
#include "Invokable.hpp"

class Class;

class Function 
    : public LanguageType
    , public Invokable
{
public:
    Function(
        const Cursor &cursor, 
        const Namespace &currentNamespace, 
        Class *parent = nullptr
   );

    virtual ~Function(void) { }

    bool ShouldCompile(void) const;

private:
    Class *m_parent;

    std::string m_name;
    std::string m_qualifiedName;

    bool isAccessible(void) const;

    std::string getQualifiedSignature(void) const;
};