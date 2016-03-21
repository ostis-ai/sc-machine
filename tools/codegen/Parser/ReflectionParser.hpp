/* ----------------------------------------------------------------------------
** © 201x Austin Brunkhorst, All Rights Reserved.
**
** ReflectionParser.h
** --------------------------------------------------------------------------*/

#pragma once

#include "ReflectionOptions.hpp"

#include "Cursor.hpp"
#include "Namespace.hpp"

#include "MacrosManager.hpp"

#include <list>

class Class;
class Global;
class Function;
class Enum;

class ReflectionParser
{
	typedef std::list<std::string> tStringList;

public:
    ReflectionParser(const ReflectionOptions &options);
    ~ReflectionParser(void);

    void Parse(void);
	void ProcessFile(std::string const & fileName);

protected:
    void DumpTree(Cursor const & cursor, int level, std::stringstream & outData);

protected:
	static void CollectFiles(std::string const & inPath, tStringList & outFiles);
	static std::string GetFileExtension(std::string const & fileName);
	static std::string GetOutputFileName(std::string const & fileName);
    static std::string GetFileID(std::string const & fileName);

private:
    ReflectionOptions m_options;

    CXIndex m_index;
    CXTranslationUnit m_translationUnit;

    std::vector<Class*> m_classes;
    std::vector<Global*> m_globals;
    std::vector<Function*> m_globalFunctions;
    std::vector<Enum*> m_enums;

    mutable std::unordered_map<std::string, std::string> m_templatePartialCache;

    void buildClasses(Cursor const & cursor, Namespace & currentNamespace, std::stringstream & outCode);
    /*void buildGlobals(const Cursor &cursor, Namespace &currentNamespace);

    void buildGlobalFunctions(
        const Cursor &cursor, 
        Namespace &currentNamespace
   );

    void buildEnums(const Cursor &cursor, Namespace &currentNamespace);*/
};
