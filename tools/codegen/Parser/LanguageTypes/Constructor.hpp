#pragma once

#include "LanguageType.hpp"
#include "Invokable.hpp"

class Class;

class Constructor 
    : public LanguageType
    , public Invokable
{
public:
    Constructor(
    	const Cursor &cursor, 
    	const Namespace &currentNamespace, 
    	Class *parent = nullptr
	);

    virtual ~Constructor(void) { } 

    bool ShouldCompile(void) const;
    
private:
    Class *m_parent;

    bool isAccessible(void) const;

    std::string getTemplateParameters(void) const;
};