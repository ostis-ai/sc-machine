#pragma once

#include "ReflectionOptions.hpp"

#include "Cursor.hpp"

#include "MacrosManager.hpp"
#include "Cache.hpp"

#include "LanguageTypes/Class.hpp"
#include "LanguageTypes/Global.hpp"
#include "LanguageTypes/Function.hpp"

class ReflectionParser final
{
public:
  explicit ReflectionParser(ReflectionOptions const & options);

  void Parse();
  /** If processed file contains module, then returns false */
  bool ProcessFile(std::string const & fileName, bool InProcessModule = false);
  void ResetCache();

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
  void buildClasses(Cursor const & cursor, Namespace & currentNamespace);

private:
  ReflectionOptions m_options;

  CXIndex m_index;
  CXTranslationUnit m_translationUnit;

  std::string m_currentFile;

  std::vector<std::unique_ptr<Class>> m_classes;
  std::vector<std::unique_ptr<Global>> m_globals;
  std::vector<std::unique_ptr<Function>> m_globalFunctions;

  std::unique_ptr<SourceCache> m_sourceCache;
};
