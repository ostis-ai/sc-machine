#include "scs_translator.h"

#include <fstream>
#include <iostream>



SCsTranslator::SCsTranslator()
{
}

SCsTranslator::~SCsTranslator()
{
}

bool SCsTranslator::translate(const std::string &filename)
{
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

const std::string& SCsTranslator::getFileExt() const
{
    static std::string ext = "scs";
    return ext;
}

bool SCsTranslator::processString(const String &data)
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

    // translate

    parser->free(parser);
    tokens->free(tokens);
    lex->free(lex);

    input->close(input);
}

bool SCsTranslator::buildScText(pANTLR3_BASE_TREE tree)
{


    return true;
}


//void Builder::dumpTree(pANTLR3_BASE_TREE tree, int level)
//{
//    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
//    if (tok)
//    {
//        switch(tok->type)
//        {
//        case ID_SYSTEM:
//            std::cout << "idtf";//tok->getText(tok);
//            break;
//        case SEP_SENTENCE:
//            std::cout << ";;";
//            break;
//        case CONNECTORS:
//            std::cout << "->";
//            break;
//        };
//    }

//    std::cout << "\n";
//    for (int i = 0; i < level; ++i)
//        std::cout << "- ";

//    std::cout << tree->getText(tree)->chars;

//    unsigned int nodesCount = tree->getChildCount(tree);
//    for (unsigned int i = 0; i < nodesCount; ++i)
//    {
//        dumpTree((pANTLR3_BASE_TREE)tree->getChild(tree, i), level + 1);
//    }

//}

// -------------
SCsTranslatorFactory::SCsTranslatorFactory()
{

}

SCsTranslatorFactory::~SCsTranslatorFactory()
{

}

iTranslator* SCsTranslatorFactory::createInstance()
{
    return new SCsTranslator();
}

const std::string& SCsTranslatorFactory::getFileExt() const
{
    static std::string ext = "scs";
    return ext;
}
