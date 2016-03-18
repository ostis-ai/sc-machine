/* ----------------------------------------------------------------------------
** © 201x Austin Brunkhorst, All Rights Reserved.
**
** ReflectionParser.h
** --------------------------------------------------------------------------*/

#pragma once

#include "ReflectionOptions.hpp"

#include "Cursor.hpp"
#include "Namespace.hpp"

#include "Templates.hpp"

class Class;
class Global;
class Function;
class Enum;

class ReflectionParser
{
public:
    ReflectionParser(const ReflectionOptions &options);
    ~ReflectionParser(void);

    void Parse(void);

    void GenerateHeader(std::string &output) const;
    void GenerateSource(std::string &output) const;

private:
    ReflectionOptions m_options;

    CXIndex m_index;
    CXTranslationUnit m_translationUnit;

    std::vector<Class*> m_classes;
    std::vector<Global*> m_globals;
    std::vector<Function*> m_globalFunctions;
    std::vector<Enum*> m_enums;

    mutable std::unordered_map<std::string, std::string> m_templatePartialCache;

    void buildClasses(const Cursor &cursor, Namespace &currentNamespace);
    void buildGlobals(const Cursor &cursor, Namespace &currentNamespace);

    void buildGlobalFunctions(
        const Cursor &cursor, 
        Namespace &currentNamespace
    );

    void buildEnums(const Cursor &cursor, Namespace &currentNamespace);
};