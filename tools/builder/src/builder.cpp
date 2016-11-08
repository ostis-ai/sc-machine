/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder.h"
#include "utils.h"
#include "translator.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <assert.h>

#include "scs_translator.h"
#include "gwf_translator.h"

Builder::Builder()
    : mContext(0)
{
}

Builder::~Builder()
{
}

void Builder::initialize()
{
    // register translator factories
    registerTranslator(new SCsTranslatorFactory());
    registerTranslator(new GwfTranslatorFactory());
}

bool Builder::run(const BuilderParams &params)
{

    mParams = params;

    collectFiles();

    // initialize sc-memory
    sc_memory_params p;
    sc_memory_params_clear(&p);
    p.clear = mParams.clearOutput ? SC_TRUE : SC_FALSE;
    p.config_file = mParams.configFile.empty() ? 0 : mParams.configFile.c_str();
    p.repo_path = mParams.outputPath.c_str();
    p.ext_path = mParams.extensionsPath.size() > 0 ? mParams.extensionsPath.c_str() : 0;

    sc_memory_initialize(&p);

    mContext = sc_memory_context_new(sc_access_lvl_make_min);

    std::cout << "Build knowledge base from sources... " << std::endl;

    // process founded files
    uint32 done = 0, last_progress = -1;
    tFileSet::iterator it, itEnd = mFileSet.end();
    for (it = mFileSet.begin(); it != itEnd; ++it)
    {
		uint32 progress = (uint32)(((float)++done / (float)mFileSet.size()) * 100);
		if (mParams.showFileNames)
		{
			std::cout << "[ " << progress << "% ] " << *it << std::endl;
		}
		else
		{
			if (last_progress != progress)
			{
				if (progress % 10 == 0)
				{
					std::cout << "[" << progress << "%]";
					std::cout.flush();
				}
				else
				{
					std::cout << ".";
					std::cout.flush();
				}
				last_progress = progress;
			}
		}

        try
        {
            processFile(*it);
        } catch(const Exception &e)
        {
            StringStream ss;
            ss << e.getDescription() << " in " << e.getFileName() << " at line " << e.getLineNumber();
            mErrors.push_back(ss.str());
        }
    }
    std::cout << std::endl << "done" << std::endl;

    // print errors
    std::cout << std::endl << "-------" << std::endl << "Errors:" << std::endl;
    int idx = 1;
    tStringList::iterator itErr, itErrEnd = mErrors.end();
    for (itErr = mErrors.begin(); itErr != itErrEnd; ++itErr)
        std::cout << "[" << idx++ << "]\t" << *itErr << std::endl;

    // print statistics
    sc_stat stat;
    sc_memory_stat(mContext, &stat);

    unsigned int all_count = stat.arc_count + stat.node_count + stat.link_count;

    std::cout << std::endl << "Statistics" << std::endl;
    std::cout << "Nodes: " << stat.node_count << "(" << ((float)stat.node_count / (float)all_count) * 100 << "%)" << std::endl;
    std::cout << "Arcs: " << stat.arc_count << "(" << ((float)stat.arc_count / (float)all_count) * 100 << "%)"  << std::endl;
    std::cout << "Links: " << stat.link_count << "(" << ((float)stat.link_count / (float)all_count) * 100 << "%)"  << std::endl;
    std::cout << "Total: " << all_count << std::endl;

    sc_memory_context_free(mContext);
    sc_memory_shutdown(SC_TRUE);

    return true;
}

void Builder::registerTranslator(iTranslatorFactory *factory)
{
    assert(!hasTranslator(factory->getFileExt()));
    mTranslatorFactories[factory->getFileExt()] = factory;
}

bool Builder::hasTranslator(const std::string &ext) const
{
    return mTranslatorFactories.find(ext) != mTranslatorFactories.end();
}
bool Builder::hasErrors() const
{
    return mErrors.size() > 0;
}

bool Builder::processFile(const String &filename)
{
    // get file extension
    size_t n = filename.rfind(".");
    if (n == std::string::npos)
    {
        THROW_EXCEPT(Exception::ERR_FILE_NOT_FOUND,
                    "Can't determine file extension " + filename,
                     filename, 0);
        return false;
    }

    std::string ext = filename.substr(n + 1, std::string::npos);
    // try to find translator factory
    tTranslatorFactories::iterator it = mTranslatorFactories.find(ext);
    if (it == mTranslatorFactories.end())
    {
        THROW_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
                     "There are no translators, that support " + ext + " extension",
                     filename, 0);
        return false;
    }

    iTranslator *translator = it->second->createInstance(mContext);
    assert(translator);

    TranslatorParams translateParams;
    translateParams.fileName = filename;
    translateParams.autoFormatInfo = mParams.autoFormatInfo;

    bool result = translator->translate(translateParams);
    delete translator;

    return result;
}

void Builder::collectFiles(const String & path)
{
    boost::filesystem::recursive_directory_iterator itEnd, it(path);
    while (it != itEnd)
    {
        if (!boost::filesystem::is_directory(*it))
        {
            boost::filesystem::path path = *it;
            String filename = path.string();
            String ext = StringUtil::getFileExtension(filename);
            StringUtil::toLowerCase(ext);

            if (mTranslatorFactories.find(ext) != mTranslatorFactories.end())
                mFileSet.insert(filename);
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
            } catch(...)
            {
                std::cout << ex.what() << std::endl;
                return;
            }
        }
    }
}

void Builder::collectFiles()
{
    mFileSet.clear();
    if (boost::filesystem::is_directory(mParams.inputPath))
    {
        collectFiles(mParams.inputPath);
    }
    else if (boost::filesystem::is_regular_file(mParams.inputPath))
    {
        std::ifstream infile;
        infile.open(mParams.inputPath.c_str());
        if (infile.is_open())
        {
            String path;
            while (std::getline(infile, path))
            {
                boost::trim(path);
                if (StringUtil::startsWith(path, "#", true))
                    continue;

                if (!path.empty())
                {
                    if (boost::filesystem::is_directory(path))
                        collectFiles(path);
                    else
                    {
                        StringStream ss;
                        ss << path << " isn't a directory";
                        mErrors.push_back(ss.str());
                    }
                }
            }
        } else
        {
            StringStream ss;
            ss << "Can't open file: " << mParams.inputPath;
            mErrors.push_back(ss.str());
        }
    }

}


