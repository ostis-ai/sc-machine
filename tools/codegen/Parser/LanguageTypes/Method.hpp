#pragma once

#include "LanguageType.hpp"
#include "Invokable.hpp"

class Class;

class Method 
    : public LanguageType
    , public Invokable
{
public:
    Method(
        const Cursor &cursor, 
        const Namespace &currentNamespace, 
        Class *parent = nullptr
   );

    virtual ~Method(void) { }

    bool ShouldCompile(void) const;
    
private:
    bool m_isConst;

    Class *m_parent;

    std::string m_name;

    bool isAccessible(void) const;

    std::string getQualifiedSignature(void) const;
};