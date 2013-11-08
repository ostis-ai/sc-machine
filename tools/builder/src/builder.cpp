#include "builder.h"
#include "utils.h"


#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>

Builder::Builder()
    : mClearOutput(false)
{
}

Builder::~Builder()
{
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
bool Builder::processString(const String &data)
{
    pANTLR3_INPUT_STREAM input;

#if defined( __WIN32__ ) || defined( _WIN32 )
    input = antlr3StringStreamNew((pANTLR3_UINT8)data.c_str(), ANTLR3_ENC_UTF8, data.length(), (pANTLR3_UINT8)"scs");
#elif defined( __APPLE_CC__)
    input = antlr3StringStreamNew((pANTLR3_UINT8)data.c_str(), ANTLR3_ENC_UTF8, data.length(), (pANTLR3_UINT8)"scs");
#else
    input = antlr3NewAsciiStringCopyStream((pANTLR3_UINT8)data.c_str(), data.length(), (pANTLR3_UINT8)"scs");
#endif

    pscsLexer lex;
    pANTLR3_COMMON_TOKEN_STREAM tokens;
    pscsParser parser;

    lex = scsLexerNew(input);
    tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT,
                                            TOKENSOURCE(lex));
    parser = scsParserNew(tokens);


    scsParser_syntax_return r = parser->syntax(parser);
    pANTLR3_BASE_TREE tree = r.tree;

    //std::cout << tree->toStringTree(tree) << std::endl;
    dumpTree(tree, 0);
    //pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    //printf("%d", tok->type);

    parser->free(parser);
    tokens->free(tokens);
    lex->free(lex);

    input->close(input);
}

bool Builder::processFile(const String &filename)
{
    std::cout << "Process: " << filename << std::endl;

    // open file and read data
    bool result = true;
    std::ifstream ifs(filename.c_str());
    if (ifs.is_open())
    {
        String data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        result = processString(data);
    } else
        return false;

    ifs.close();

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

sc_addr Builder::createLink(const std::string &str)
{
    boost::addable
}

void Builder::dumpTree(pANTLR3_BASE_TREE tree, int level)
{
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    if (tok)
    {
        switch(tok->type)
        {
        case ID_SYSTEM:
            std::cout << "idtf";//tok->getText(tok);
            break;
        case SEP_SENTENCE:
            std::cout << ";;";
            break;
        case CONNECTORS:
            std::cout << "->";
            break;
        };
    }

    std::cout << "\n";
    for (int i = 0; i < level; ++i)
        std::cout << "- ";

    std::cout << tree->getText(tree)->chars;

    unsigned int nodesCount = tree->getChildCount(tree);
    for (unsigned int i = 0; i < nodesCount; ++i)
    {
        dumpTree((pANTLR3_BASE_TREE)tree->getChild(tree, i), level + 1);
    }

}
