#include "Precompiled.hpp"

#include "ReflectionParser.hpp"

#include "LanguageTypes/Class.hpp"
#include "LanguageTypes/Global.hpp"
#include "LanguageTypes/Function.hpp"
#include "LanguageTypes/Enum.hpp"

#include <iostream>
#include <fstream>

#include <boost/algorithm/string.hpp>

#define RECURSE_NAMESPACES(kind, cursor, method, ns) \
    if (kind == CXCursor_Namespace)                  \
    {                                                \
        auto displayName = cursor.GetDisplayName(); \
        if (!displayName.empty())                   \
        {                                            \
            ns.emplace_back(displayName);          \
            method(cursor, ns);                    \
            ns.pop_back();                          \
        }                                            \
    }                                                \

ReflectionParser::ReflectionParser(const ReflectionOptions &options)
    : m_options(options)
    , m_index(nullptr)
    , m_translationUnit(nullptr)
{
   
}

ReflectionParser::~ReflectionParser(void)
{
    for (auto *klass : m_classes)
        delete klass;

    for (auto *global : m_globals)
        delete global;

    for (auto *globalFunction : m_globalFunctions)
        delete globalFunction;

    for (auto *enewm : m_enums)
        delete enewm;

    if (m_translationUnit)
        clang_disposeTranslationUnit(m_translationUnit);

    if (m_index)
        clang_disposeIndex(m_index);
}

void ReflectionParser::CollectFiles(std::string const & inPath, ReflectionParser::tStringList & outFiles)
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

			if ((ext == "hpp") || (ext == "h"))
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

void ReflectionParser::Parse(void)
{
	tStringList filesList;
	CollectFiles(m_options.inputPath, filesList);

	// ensure that output directory exist
	fs::create_directory(fs::path(m_options.outputPath));

	for (tStringList::const_iterator it = filesList.begin(); it != filesList.end(); ++it)
	{
		std::cout << "Parse header " << *it << "..." << std::endl;
		ProcessFile(*it);
	}
}

void ReflectionParser::ProcessFile(std::string const & fileName)
{
	m_index = clang_createIndex(true, false);

	std::vector<const char *> arguments;

	for (auto &argument : m_options.arguments)
	{
		// unescape flags
		boost::algorithm::replace_all(argument, "\\-", "-");

		arguments.emplace_back(argument.c_str());
	}

	m_translationUnit = clang_createTranslationUnitFromSourceFile(
		m_index,
		fileName.c_str(),
		static_cast<int>(arguments.size()),
		arguments.data(),
		0,
		nullptr
		);

	auto cursor = clang_getTranslationUnitCursor(m_translationUnit);

	/*Namespace tempNamespace;

	buildClasses(cursor, tempNamespace);

	tempNamespace.clear();

	buildGlobals(cursor, tempNamespace);

	tempNamespace.clear();

	buildGlobalFunctions(cursor, tempNamespace);

	tempNamespace.clear();

	buildEnums(cursor, tempNamespace);*/

	// generate output file
	fs::path outputPath(m_options.outputPath);
	outputPath /= fs::path(GetOutputFileName(fileName));
	std::ofstream outputFile(outputPath.string());
	
	outputFile.close();
}

void ReflectionParser::GenerateHeader(std::string &output) const
{
    //TemplateData headerData { TemplateData::Type::Object };

    //// general options

    //headerData[ "targetName" ] = m_options.targetName;
    //headerData[ "inputSourceFile" ] = m_options.inputSourceFile;
    //headerData[ "ouputHeaderFile" ] = m_options.outputHeaderFile;
    //headerData[ "outpuSourceFile" ] = m_options.outputSourceFile;

    //headerData[ "usingPrecompiledHeader" ] = 
    //    utils::TemplateBool(!m_options.precompiledHeader.empty());

    //headerData[ "precompiledHeader" ] = m_options.precompiledHeader;

    //auto headerTemplate = LoadTemplate(kTemplateHeader);

    //if (!headerTemplate.isValid())
    //{
    //    std::stringstream error;

    //    error << "Unable to compile header template." << std::endl;
    //    error << headerTemplate.errorMessage();

    //    throw std::exception(error.str().c_str());
    //}

    //output = headerTemplate.render(headerData);
}

void ReflectionParser::GenerateSource(std::string &output) const
{
    //TemplateData sourceData { TemplateData::Type::Object };

    //// general options

    //sourceData[ "targetName" ] = m_options.targetName;
    //sourceData[ "inputSourceFile" ] = m_options.inputSourceFile;
    //sourceData[ "ouputHeaderFile" ] = m_options.outputHeaderFile;
    //sourceData[ "outpuSourceFile" ] = m_options.outputSourceFile;

    //sourceData[ "usingPrecompiledHeader" ] = 
    //    utils::TemplateBool(!m_options.precompiledHeader.empty());

    //sourceData[ "precompiledHeader" ] = m_options.precompiledHeader;

    //// language type data

    //sourceData[ "class" ] = compileClassTemplates();
    //sourceData[ "global" ] = compileGlobalTemplates();
    //sourceData[ "globalFunction" ] = compileGlobalFunctionTemplates();
    //sourceData[ "enum" ] = compileEnumTemplates();

    //auto sourceTemplate = LoadTemplate(kTemplateSource);

    //if (!sourceTemplate.isValid())
    //{
    //    std::stringstream error;

    //    error << "Unable to compile header template." << std::endl;
    //    error << sourceTemplate.errorMessage();

    //    throw std::exception(error.str().c_str());
    //}

    //output = sourceTemplate.render(sourceData);
}

void ReflectionParser::buildClasses(const Cursor &cursor, Namespace &currentNamespace)
{
    for (auto &child : cursor.GetChildren())
    {
        auto kind = child.GetKind();

        // actual definition and a class or struct
        if (child.IsDefinition() && 
            (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl)
       )
        {
            m_classes.emplace_back(
                new Class(child, currentNamespace)
           );
        }
        
        RECURSE_NAMESPACES(kind, child, buildClasses, currentNamespace);
    }
}

void ReflectionParser::buildGlobals(const Cursor &cursor, Namespace &currentNamespace)
{
    for (auto &child : cursor.GetChildren())
    {
        // skip static globals (hidden)
        if (child.GetStorageClass() == CX_SC_Static)
            continue;

        auto kind = child.GetKind();

        // variable declaration, which is global
        if (kind == CXCursor_VarDecl) 
        {
            m_globals.emplace_back(
                new Global(child, currentNamespace) 
           );
        }

        RECURSE_NAMESPACES(kind, child, buildGlobals, currentNamespace);
    }
}

void ReflectionParser::buildGlobalFunctions(const Cursor &cursor, Namespace &currentNamespace)
{
    for (auto &child : cursor.GetChildren())
    {
        // skip static globals (hidden)
        if (child.GetStorageClass() == CX_SC_Static)
            continue;

        auto kind = child.GetKind();

        // function declaration, which is global
        if (kind == CXCursor_FunctionDecl) 
        {
            m_globalFunctions.emplace_back(new Function(child, currentNamespace));
        }

        RECURSE_NAMESPACES(
            kind, 
            child, 
            buildGlobalFunctions, 
            currentNamespace 
       );
    }
}

void ReflectionParser::buildEnums(const Cursor &cursor, Namespace &currentNamespace)
{
    for (auto &child : cursor.GetChildren())
    {
        auto kind = child.GetKind();

        // actual definition and an enum
        if (child.IsDefinition() && kind == CXCursor_EnumDecl)
        {
            // anonymous enum if the underlying type display name contains this
            if (child.GetType().GetDisplayName().find("anonymous enum at") 
                != std::string::npos)
            {
                // anonymous enums are just loaded as 
                // globals with each of their values
                Enum::LoadAnonymous(m_globals, child, currentNamespace);
            }
            else
            {
                m_enums.emplace_back(
                    new Enum(child, currentNamespace) 
               );
            }
        }

        RECURSE_NAMESPACES(kind, child, buildEnums, currentNamespace);
    }
}

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
	fs::path inputPath(fileName);
	std::string baseName = inputPath.filename().replace_extension().string();
	return baseName + ".generated" + inputPath.extension().string();
}