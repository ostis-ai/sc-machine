#include "builder.h"
#include "utils.h"
#include "translator.h"

extern "C"
{
#include "sc_memory_headers.h"
#include "sc_helper.h"
}

#include <boost/filesystem.hpp>
#include <assert.h>

#include "scs_translator.h"


Builder::Builder()
    : mClearOutput(false)
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

bool Builder::run(const String &inputPath, const String &outputPath, bool clearOutput)
{
    mInputPath = inputPath;
    mOutputPath = outputPath;
    mClearOutput = clearOutput;

    collectFiles();

    if (clearOutput)
    {
        boost::filesystem::path path(outputPath);

        if (boost::filesystem::exists(path) && boost::filesystem::is_directory(path) && !boost::filesystem::is_empty(path))
        {
            std::cout << "Clear output directory\n";
            boost::filesystem::remove_all(outputPath);

            boost::filesystem::create_directory(path);
            assert(boost::filesystem::is_empty(path));
        }

    }

    // initialize sc-memory
    sc_memory_initialize(outputPath.c_str(), 0);
    sc_helper_init();

    // print founded files
    tFileSet::iterator it, itEnd = mFileSet.end();
    for (it = mFileSet.begin(); it != itEnd; ++it)
        processFile(*it);

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
    std::cout << "Process: " << filename << std::endl;

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

    bool result = translator->translate(filename);
    delete translator;

    return result;
}

void Builder::collectFiles()
{
    boost::filesystem::recursive_directory_iterator itEnd, it(mInputPath);
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


