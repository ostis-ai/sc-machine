#pragma once

#include "LanguageType.hpp"

class Class;

class Field : public LanguageType
{
public:
    Field(
        const Cursor &cursor, 
        const Namespace &currentNamespace, 
        Class *parent = nullptr
   );

    virtual ~Field(void) { }

    bool ShouldCompile(void) const;

    void GenarateInitCode(std::stringstream & outCode) const;

	static void GenerateResolveKeynodeCode(std::string const & sysIdtf, std::string const & displayName,
		bool forceCreation, std::stringstream & outCode);
	static void GenerateTemplateBuildCode(std::string const & sysIdtf, std::string const & displayName,
		std::stringstream & outCode);

    std::string const & GetDisplayName() const;
    
private:
    bool isAccessible(void) const;
    bool isGetterAccessible(void) const;
    bool isSetterAccessible(void) const;

private:
    bool m_isConst;

    bool m_hasExplicitGetter;
    bool m_hasExplicitSetter;

    Class *m_parent;

    std::string m_name;
    std::string m_displayName;
    std::string m_type;   
};