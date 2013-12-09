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

    // print founded files
    uint32 done = 0;
    tFileSet::iterator it, itEnd = mFileSet.end();
    for (it = mFileSet.begin(); it != itEnd; ++it)
    {
        float progress = (float)++done / (float)mFileSet.size();
        std::cout << "[" << (int) (progress * 100.f) << "%] " << *it << std::endl;
        processFile(*it);
    }

    // print statistics
    sc_stat stat;
    sc_memory_stat(&stat);

    unsigned int all_count = stat.arc_count + stat.node_count + stat.link_count;

    std::cout << std::endl << "Statistics" << std::endl;
    std::cout << "Nodes: " << stat.node_count << "(" << ((float)stat.node_count / (float)all_count) * 100 << "%)" << std::endl;
    std::cout << "Arcs: " << stat.arc_count << "(" << ((float)stat.arc_count / (float)all_count) * 100 << "%)"  << std::endl;
    std::cout << "Links: " << stat.link_count << "(" << ((float)stat.link_count / (float)all_count) * 100 << "%)"  << std::endl;
    std::cout << "Total: " << all_count << std::endl;

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


