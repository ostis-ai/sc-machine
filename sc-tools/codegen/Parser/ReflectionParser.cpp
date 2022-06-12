#include "ReflectionParser.hpp"

#include <iostream>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#define RECURSE_NAMESPACES(kind, cursor, method, ns) \
  if (kind == CXCursor_Namespace) \
  { \
    auto displayName = cursor.GetDisplayName(); \
    if (!displayName.empty()) \
    { \
      ns.emplace_back(displayName); \
      method(cursor, ns); \
      ns.pop_back(); \
    } \
  }

namespace impl
{
void displayDiagnostics(CXTranslationUnit tu)
{
  if (tu == 0)
  {
    std::cerr << "Parsing error!" << std::endl;
    return;
  }

  int const numDiagnostics = clang_getNumDiagnostics(tu);
  for (int i = 0; i < numDiagnostics; ++i)
  {
    auto const diagnostic = clang_getDiagnostic(tu, i);
    auto const string = clang_formatDiagnostic(diagnostic, clang_defaultDiagnosticDisplayOptions());

    std::cerr << clang_getCString(string) << std::endl;
    clang_disposeString(string);
    clang_disposeDiagnostic(diagnostic);
  }
}

}  // namespace impl

ReflectionParser::ReflectionParser(const ReflectionOptions & options)
  : m_options(options)
  , m_index(nullptr)
  , m_translationUnit(nullptr)
  , m_sourceCache(nullptr)
{
}

void ReflectionParser::CollectFiles(std::string const & inPath, tStringList & outFiles)
{
  boost::filesystem::recursive_directory_iterator itEnd, it(inPath);
  while (it != itEnd)
  {
    if (!boost::filesystem::is_directory(*it))
    {
      boost::filesystem::path path = *it;
      std::string filename = path.string();
      std::string ext = GetFileExtension(filename);
      std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

      if (((ext == "hpp") || (ext == "h")) && (filename.find(".generated.") == std::string::npos))
      {
        outFiles.push_back(filename);
      }
    }

    try
    {
      ++it;
    }
    catch (std::exception & ex)
    {
      std::cout << ex.what() << std::endl;
      it.no_push();
      try
      {
        ++it;
      }
      catch (...)
      {
        std::cout << ex.what() << std::endl;
        return;
      }
    }
  }
}

void ReflectionParser::Parse()
{
  tStringList filesList;
  std::string moduleFile;
  CollectFiles(m_options.inputPath, filesList);

  if (m_options.useCache)
  {
    m_sourceCache.reset(new SourceCache(m_options.buildDirectory, m_options.targetName));
    m_sourceCache->Load();
    m_sourceCache->CheckGenerator(m_options.generatorPath);
  }

  if (m_options.displayDiagnostic)
  {
    std::cout << "Flags: " << std::endl;
    for (auto const & f : m_options.arguments)
      std::cout << f << std::endl;
  }

  // ensure that output directory exist
  boost::filesystem::create_directory(boost::filesystem::path(m_options.outputPath));

  for (tStringList::const_iterator it = filesList.begin(); it != filesList.end(); ++it)
  {
    try
    {
      // if contains module, then process it later
      if (!ProcessFile(*it))
      {
        if (!moduleFile.empty())
        {
          EMIT_ERROR("You couldn't implement two module classes in one module");
        }

        moduleFile = *it;
      }
    }
    catch (Exception e)
    {
      EMIT_ERROR(e.GetDescription() << " in " << *it);
    }
  }

  try
  {
    if (!moduleFile.empty())
      ProcessFile(moduleFile, true);
  }
  catch (Exception e)
  {
    EMIT_ERROR(e.GetDescription() << " in " << moduleFile);
  }

  if (m_options.useCache)
    m_sourceCache->Save();
}

void ReflectionParser::Clear()
{
  m_currentFile = "";

  m_classes.clear();
  m_globals.clear();
  m_globalFunctions.clear();
}

bool ReflectionParser::ProcessFile(std::string const & fileName, bool inProcessModule)
{
  if (!inProcessModule && m_options.useCache && !m_sourceCache->RequestGenerate(fileName))
    return true;

  if (m_options.displayDiagnostic)
    std::cout << "Processing file: " << fileName << std::endl;

  Clear();

  m_currentFile = fileName;
  m_index = clang_createIndex(true, false);

  std::vector<const char *> arguments;

  for (auto & argument : m_options.arguments)
  {
    // unescape flags
    boost::algorithm::replace_all(argument, "\\-", "-");
    arguments.emplace_back(argument.c_str());
  }

  m_translationUnit = clang_createTranslationUnitFromSourceFile(
      m_index, fileName.c_str(), static_cast<int>(arguments.size()), arguments.data(), 0, nullptr);

  if (m_options.displayDiagnostic)
    impl::displayDiagnostics(m_translationUnit);

  auto cursor = clang_getTranslationUnitCursor(m_translationUnit);

  try
  {
    Namespace tempNamespace;
    buildClasses(cursor, tempNamespace);
    tempNamespace.clear();

    if (ContainsModule() && !inProcessModule)
    {
      if (m_classes.size() > 1)
      {
        EMIT_ERROR("You can't implement any other classes in one file with module class");
      }
      return false;
    }

    if (RequestGenerate())
    {
      std::string fileId = GetFileID(fileName);
      std::stringstream outCode;

      // includes
      outCode << "#include <memory>\n\n";
      outCode << "#include \"sc-memory/sc_memory.hpp\"\n\n\n";
      outCode << "#include \"sc-memory/sc_event.hpp\"\n\n\n";

      for (auto const & klass : m_classes)
      {
        if (klass->ShouldGenerate())
          klass->GenerateCode(fileId, outCode, this);
      }

      /// write ScFileID definition
      outCode << "\n\n#undef ScFileID\n";
      outCode << "#define ScFileID " << fileId;

      // generate output file
      boost::filesystem::path outputPath(m_options.outputPath);
      outputPath /= boost::filesystem::path(GetOutputFileName(fileName));
      std::ofstream outputFile(outputPath.string());
      outputFile << outCode.str();
      outputFile << std::endl << std::endl;
      outputFile.close();
    }

    clang_disposeIndex(m_index);
    clang_disposeTranslationUnit(m_translationUnit);
  }
  catch (Exception e)
  {
    clang_disposeIndex(m_index);
    clang_disposeTranslationUnit(m_translationUnit);

    EMIT_ERROR(e.GetDescription());
  }

  return true;
}

void ReflectionParser::ResetCache()
{
  if (m_sourceCache)
    m_sourceCache->Reset();
}

bool ReflectionParser::IsInCurrentFile(Cursor const & cursor) const
{
  return cursor.GetFileName() == m_currentFile;
}

bool ReflectionParser::RequestGenerate() const
{
  for (auto it = m_classes.begin(); it != m_classes.end(); ++it)
  {
    if ((*it)->ShouldGenerate())
      return true;
  }

  return false;
}

bool ReflectionParser::ContainsModule() const
{
  for (auto it = m_classes.begin(); it != m_classes.end(); ++it)
  {
    if ((*it)->IsModule())
      return true;
  }

  return false;
}

void ReflectionParser::buildClasses(const Cursor & cursor, Namespace & currentNamespace)
{
  for (auto & child : cursor.GetChildren())
  {
    // skip classes from other files
    if (!IsInCurrentFile(child))
      continue;

    auto const kind = child.GetKind();

    // actual definition and a class or struct
    if (child.IsDefinition() && (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl))
      m_classes.emplace_back(new Class(child, currentNamespace));

    RECURSE_NAMESPACES(kind, child, buildClasses, currentNamespace);
  }
}

void ReflectionParser::DumpTree(Cursor const & cursor, size_t level, std::stringstream & outData)
{
  outData << "\n";
  for (size_t i = 0; i < level; ++i)
    outData << "-";

  outData << cursor.GetDisplayName() << ", " << cursor.GetKind();

  for (auto & child : cursor.GetChildren())
  {
    DumpTree(child, level + 1, outData);
  }
}

// void ReflectionParser::buildGlobals(const Cursor &cursor, Namespace &currentNamespace)
//{
//     for (auto &child : cursor.GetChildren())
//     {
//         // skip static globals (hidden)
//         if (child.GetStorageClass() == CX_SC_Static)
//             continue;
//
//         auto kind = child.GetKind();
//
//         // variable declaration, which is global
//         if (kind == CXCursor_VarDecl)
//         {
//             m_globals.emplace_back(
//                 new Global(child, currentNamespace)
//            );
//         }
//
//         RECURSE_NAMESPACES(kind, child, buildGlobals, currentNamespace);
//     }
// }
//
// void ReflectionParser::buildGlobalFunctions(const Cursor &cursor, Namespace &currentNamespace)
//{
//     for (auto &child : cursor.GetChildren())
//     {
//         // skip static globals (hidden)
//         if (child.GetStorageClass() == CX_SC_Static)
//             continue;
//
//         auto kind = child.GetKind();
//
//         // function declaration, which is global
//         if (kind == CXCursor_FunctionDecl)
//         {
//             m_globalFunctions.emplace_back(new Function(child, currentNamespace));
//         }
//
//         RECURSE_NAMESPACES(
//             kind,
//             child,
//             buildGlobalFunctions,
//             currentNamespace
//        );
//     }
// }
//
// void ReflectionParser::buildEnums(const Cursor &cursor, Namespace &currentNamespace)
//{
//     for (auto &child : cursor.GetChildren())
//     {
//         auto kind = child.GetKind();
//
//         // actual definition and an enum
//         if (child.IsDefinition() && kind == CXCursor_EnumDecl)
//         {
//             // anonymous enum if the underlying type display name contains this
//             if (child.GetType().GetDisplayName().find("anonymous enum at")
//                 != std::string::npos)
//             {
//                 // anonymous enums are just loaded as
//                 // globals with each of their values
//                 Enum::LoadAnonymous(m_globals, child, currentNamespace);
//             }
//             else
//             {
//                 m_enums.emplace_back(
//                     new Enum(child, currentNamespace)
//                );
//             }
//         }
//
//         RECURSE_NAMESPACES(kind, child, buildEnums, currentNamespace);
//     }
// }

std::string ReflectionParser::GetFileExtension(std::string const & fileName)
{
  // get file extension
  size_t n = fileName.rfind(".");
  if (n == std::string::npos)
    return std::string();

  return fileName.substr(n + 1, std::string::npos);
}

std::string ReflectionParser::GetOutputFileName(std::string const & fileName)
{
  boost::filesystem::path const inputPath(fileName);
  std::string baseName = inputPath.filename().replace_extension().string();
  return baseName + ".generated" + inputPath.extension().string();
}

std::string ReflectionParser::GetFileID(std::string const & fileName)
{
  boost::filesystem::path inputPath(fileName);
  std::string res = inputPath.filename().string();

  for (std::string::iterator it = res.begin(); it != res.end(); ++it)
  {
    if (*it == ' ' || *it == '-' || *it == '.')
    {
      *it = '_';
    }
  }

  return res;
}
