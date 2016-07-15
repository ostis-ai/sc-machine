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
#include "Cache.hpp"

#include <list>

class ReflectionParser
{
public:

    ReflectionParser(const ReflectionOptions &options);
    ~ReflectionParser(void);

    void Parse(void);
	/** If processed file contains module, then returns false */
	bool ProcessFile(std::string const & fileName, bool InProcessModule = false);

protected:
    void Clear();
    void DumpTree(Cursor const & cursor, size_t level, std::stringstream & outData);
    bool IsInCurrentFile(Cursor const & cursor) const;
	bool RequestGenerate() const;
	bool ContainsModule() const;

protected:
	static void CollectFiles(std::string const & inPath, tStringList & outFiles);
	static std::string GetFileExtension(std::string const & fileName);
	static std::string GetOutputFileName(std::string const & fileName);
    static std::string GetFileID(std::string const & fileName);

private:
    ReflectionOptions m_options;

    CXIndex m_index;
    CXTranslationUnit m_translationUnit;

    std::string m_currentFile;

    std::vector<class Class*> m_classes;
    std::vector<class Global*> m_globals;
    std::vector<class Function*> m_globalFunctions;

	SourceCache * m_sourceCache;

	void buildClasses(Cursor const & cursor, Namespace & currentNamespace);
    /*void buildGlobals(const Cursor &cursor, Namespace &currentNamespace);

    void buildGlobalFunctions(
        const Cursor &cursor, 
        Namespace &currentNamespace
   );

    void buildEnums(const Cursor &cursor, Namespace &currentNamespace);*/
};
