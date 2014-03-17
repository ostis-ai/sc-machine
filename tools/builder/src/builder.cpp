/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "builder.h"
#include "utils.h"
#include "translator.h"

#include <boost/filesystem.hpp>
#include <assert.h>

#include "scs_translator.h"

Builder::Builder()
{
}

Builder::~Builder()
{
}

void Builder::initialize()
{
    // register translator factories
    registerTranslator(new SCsTranslatorFactory());
}

bool Builder::run(const BuilderParams &params)
{

    mParams = params;

    collectFiles();

    // initialize sc-memory
    sc_memory_initialize(mParams.outputPath.c_str(), mParams.configFile.empty() ? 0 : mParams.configFile.c_str(), mParams.clearOutput ? SC_TRUE : SC_FALSE);
    sc_helper_init();
    if (mParams.extensionsPath.size() > 0)
        sc_memory_initialize_ext(mParams.extensionsPath.c_str());

    // print founded files
    uint32 done = 0;
    tFileSet::iterator it, itEnd = mFileSet.end();
    for (it = mFileSet.begin(); it != itEnd; ++it)
    {
        float progress = (float)++done / (float)mFileSet.size();
        std::cout << "[" << (int) (progress * 100.f) << "%] " << *it << std::endl;
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

    // print errors
    std::cout << std::endl << "-------" << std::endl << "Errors:" << std::endl;
    int idx = 1;
    tStringList::iterator itErr, itErrEnd = mErrors.end();
    for (itErr = mErrors.begin(); itErr != itErrEnd; ++itErr)
        std::cout << "[" << idx++ << "]\t" << *itErr << std::endl;

    // print statistics
    sc_stat stat;
    sc_memory_stat(&stat);

    unsigned int all_count = stat.arc_count + stat.node_count + stat.link_count;

    std::cout << std::endl << "Statistics" << std::endl;
    std::cout << "Nodes: " << stat.node_count << "(" << ((float)stat.node_count / (float)all_count) * 100 << "%)" << std::endl;
    std::cout << "Arcs: " << stat.arc_count << "(" << ((float)stat.arc_count / (float)all_count) * 100 << "%)"  << std::endl;
    std::cout << "Links: " << stat.link_count << "(" << ((float)stat.link_count / (float)all_count) * 100 << "%)"  << std::endl;
    std::cout << "Total: " << all_count << std::endl;

    if (mParams.extensionsPath.size() > 0)
        sc_memory_shutdown_ext();

    sc_memory_shutdown();

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

bool Builder::processFile(const String &filename)
{
    // get file extension
    size_t n = filename.rfind(".");
    if (n == std::string::npos)
    {
        std::cout << "\tCan't determine file extension" << std::endl;
        return false;
    }

    std::string ext = filename.substr(n + 1, std::string::npos);
    // try to find translator factory
    tTranslatorFactories::iterator it = mTranslatorFactories.find(ext);
    if (it == mTranslatorFactories.end())
    {
        std::cout << "\tThere are no translators, that support " << ext << " extension" << std::endl;
        return false;
    }

    iTranslator *translator = it->second->createInstance();
    assert(translator);

    TranslatorParams translateParams;
    translateParams.fileName = filename;
    translateParams.autoFormatInfo = mParams.autoFormatInfo;

    bool result = translator->translate(translateParams);
    delete translator;

    return result;
}

void Builder::collectFiles()
{
    boost::filesystem::recursive_directory_iterator itEnd, it(mParams.inputPath);
    while (it != itEnd)
    {
        if (!boost::filesystem::is_directory(*it))
        {
            boost::filesystem::path path = *it;
            String filename = path.string();

            if (StringUtil::endsWith(filename, "scs", true))
                mFileSet.insert(filename);
        }


        try
        {
            ++it; // 5.
        }
        catch(std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
            it.no_push(); // 6.
            try { ++it; } catch(...) { std::cout << "!!" << std::endl; return; } // 7.
        }
    }

}


