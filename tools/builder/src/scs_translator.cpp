#include "scs_translator.h"

#include <fstream>
#include <iostream>
#include <assert.h>

SCsTranslator::tStringAddrMap SCsTranslator::msGlobalIdtfAddrs = SCsTranslator::tStringAddrMap();
uint32 SCsTranslator::msIdCounter = 1;

// ------------------------

SCsTranslator::SCsTranslator()
{
}

SCsTranslator::~SCsTranslator()
{
    // destroy element descrptions
    tElementIdMap::iterator it, itEnd = mElementIds.end();
    for (it = mElementIds.begin(); it != itEnd; ++it)
        delete it->second;
    mElementIds.clear();
}

bool SCsTranslator::translate(const String &filename)
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

const String& SCsTranslator::getFileExt() const
{
    static String ext = "scs";
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
    buildScText(tree);
    dumpDot(tree);

    parser->free(parser);
    tokens->free(tokens);
    lex->free(lex);

    input->close(input);
}

bool SCsTranslator::buildScText(pANTLR3_BASE_TREE tree)
{
    int nodesCount = tree->getChildCount(tree);
    for (int i = 0; i < nodesCount; ++i)
    {
        pANTLR3_BASE_TREE sentenceNode = (pANTLR3_BASE_TREE)tree->getChild(tree, i);
        eSentenceType sentenceType = determineSentenceType(sentenceNode);
        switch (sentenceType)
        {
        case SentenceLevel1:
            processSentenceLevel1(sentenceNode);
            break;
        case SentenceLevel2_7:
            processSentenceLevel2_7(sentenceNode);
            break;
        case SentenceAssign:
            break;
        default:
            std::cerr << "Unknown sentence type. Sentence " << i + 1 << std::endl;
            break;
        }

        pANTLR3_COMMON_TOKEN tok = sentenceNode->getToken(sentenceNode);
        std::cout << tok->type << std::endl;
    }

    return true;
}

SCsTranslator::eSentenceType SCsTranslator::determineSentenceType(pANTLR3_BASE_TREE node)
{
    unsigned int nodesCount = node->getChildCount(node);
    if (nodesCount == 5)
        return SentenceLevel1;

    if (nodesCount == 3)
    {
        pANTLR3_BASE_TREE sep = (pANTLR3_BASE_TREE)node->getChild(node, 1);
        pANTLR3_COMMON_TOKEN tok = sep->getToken(sep);

        if (tok->type == SEP_ASSIGN)
            return SentenceAssign;

        return SentenceLevel2_7;
    }

    return SentenceUnknown;
}

void SCsTranslator::processSentenceLevel1(pANTLR3_BASE_TREE node)
{
    unsigned int nodesCount = node->getChildCount(node);
    assert(nodesCount == 5);

    pANTLR3_BASE_TREE sep = (pANTLR3_BASE_TREE)node->getChild(node, 1);
    pANTLR3_COMMON_TOKEN tok = sep->getToken(sep);

    assert(tok->type == SEP_SIMPLE);

    sep = (pANTLR3_BASE_TREE)node->getChild(node, 3);
    tok = sep->getToken(sep);

    assert(tok->type == SEP_SIMPLE);


}

void SCsTranslator::processSentenceLevel2_7(pANTLR3_BASE_TREE node)
{

}

sc_addr SCsTranslator::resolveScAddr(const String &idtf)
{
    // try to find in system identifiers
    tStringAddrMap::iterator it = mSysIdtfAddrs.find(idtf);
    if (it != mSysIdtfAddrs.end())
        return it->second;

    // try to find in global identifiers
    it = msGlobalIdtfAddrs.find(idtf);
    if (it != msGlobalIdtfAddrs.end())
        return it->second;

    // try to find in local identifiers
    it = mLocalIdtfAddrs.find(idtf);
    if (it != mLocalIdtfAddrs.end())
        return it->second;

    // resolve system identifier
    sc_addr addr;
    sc_result res = sc_helper_find_element_by_system_identifier(idtf.c_str(), idtf.size(), &addr);
    if (res == SC_RESULT_OK)
    {
        mSysIdtfAddrs[idtf] = addr;
        return addr;
    }


}


uint32 SCsTranslator::_addNode(const String &idtf)
{
    sElement *el = new sElement();
    memset(el, 0, sizeof(sElement));

    el->idtf = idtf;
    el->type = sc_type_node;
    el->id = msIdCounter++;

    assert(mElementIds.find(el->id) == mElementIds.end());
    mElementIds[el->id] = el;
}

uint32 SCsTranslator::_addEdge(sElement *source, sElement *target, sc_type type, const String &idtf)
{
    assert(source && target);

    sElement *el = new sElement();
    memset(el, 0, sizeof(sElement));

    el->idtf = idtf;
    el->type = type;
    el->id = msIdCounter++;

    el->arc_src = source;
    el->arc_trg = target;

    assert(mElementIds.find(el->id) == mElementIds.end());
    mElementIds[el->id] = el;
}

uint32 SCsTranslator::_addLink(bool is_file, const String &data)
{
    sElement *el = new sElement();
    memset(el, 0, sizeof(sElement));

    el->type = sc_type_link;
    el->id = msIdCounter++;

    el->link_is_file = is_file;
    el->link_data = data;

    assert(mElementIds.find(el->id) == mElementIds.end());
    mElementIds[el->id] = el;
}


sc_type SCsTranslator::_getArcPreffixType(const String &preffix) const
{
    // do not use map, to prevent it initialization on class creation
    if (preffix == "sc_arc_common")
        return sc_type_arc_common;

    if (preffix == "sc_arc_main")
        return sc_type_arc_pos_const_perm;

    if (preffix == "sc_edge")
        return sc_type_edge_common;

    if (preffix == "sc_arc_access")
        return sc_type_arc_access;

    return 0;
}

sc_type SCsTranslator::_getTypeBySetIdtf(const String &setIdtf) const
{
    if (setIdtf == "sc_edge_const")
        return sc_type_edge_common | sc_type_const;
    if (setIdtf == "sc_edge_var")
        return sc_type_edge_common | sc_type_var;
    if (setIdtf == "sc_arc_common_const")
        return sc_type_arc_common | sc_type_const;
    if (setIdtf == "sc_arc_common_var")
        return sc_type_arc_common | sc_type_var;

    if (setIdtf == "sc_arc_access_var_pos_perm")
        return sc_type_arc_access | sc_type_var | sc_type_arc_pos | sc_type_arc_perm;
    if (setIdtf == "sc_arc_access_const_neg_perm")
        return sc_type_arc_access | sc_type_const | sc_type_arc_neg | sc_type_arc_perm;
    if (setIdtf == "sc_arc_access_var_neg_perm")
        return sc_type_arc_access | sc_type_var | sc_type_arc_neg | sc_type_arc_perm;
    if (setIdtf == "sc_arc_access_const_fuz_perm")
        return sc_type_arc_access | sc_type_const | sc_type_arc_fuz | sc_type_arc_perm;
    if (setIdtf == "sc_arc_access_var_fuz_perm")
        return sc_type_arc_access | sc_type_var | sc_type_arc_fuz | sc_type_arc_perm;

    if (setIdtf == "sc_arc_access_const_pos_temp")
        return sc_type_arc_access | sc_type_const | sc_type_arc_pos | sc_type_arc_temp;
    if (setIdtf == "sc_arc_access_var_pos_temp")
        return sc_type_arc_access | sc_type_var | sc_type_arc_pos | sc_type_arc_temp;
    if (setIdtf == "sc_arc_access_const_neg_temp")
        return sc_type_arc_access | sc_type_const | sc_type_arc_neg | sc_type_arc_temp;
    if (setIdtf == "sc_arc_access_var_neg_temp")
        return sc_type_arc_access | sc_type_var | sc_type_arc_neg | sc_type_arc_temp;
    if (setIdtf == "sc_arc_access_const_fuz_temp")
        return sc_type_arc_access | sc_type_const | sc_type_arc_fuz | sc_type_arc_temp;
    if (setIdtf == "sc_arc_access_var_fuz_temp")
        return sc_type_arc_access | sc_type_var | sc_type_arc_fuz | sc_type_arc_temp;

    if (setIdtf == "sc_const")
        return sc_type_const;
    if (setIdtf == "sc_var")
        return sc_type_var;
    if (setIdtf == "sc_node_not_binary_tuple")
        return sc_type_node_tuple;
    if (setIdtf == "sc_node_struct")
        return sc_type_node_struct;
    if (setIdtf == "sc_node_role_relation")
        return sc_type_node_role;
    if (setIdtf == "sc_node_norole_relation")
        return sc_type_node_norole;
    if (setIdtf == "sc_node_not_relation")
        return sc_type_node_class;
    if (setIdtf == "sc_node_abstract")
        return sc_type_node_abstract;
    if (setIdtf == "sc_node_material")
        return sc_type_node_material;

    return 0;
}

void SCsTranslator::dumpDot(pANTLR3_BASE_TREE tree)
{
    std::ofstream out("test.dot");

    out << "digraph g {" << std::endl;
    dumpNode(tree, out);
    out << "}" << std::endl;

    out.close();
}

void SCsTranslator::dumpNode(pANTLR3_BASE_TREE node, std::ofstream &stream)
{
    StringStream ss;
    ss << node;

    String s_root = ss.str().substr(3);
    pANTLR3_COMMON_TOKEN tok = node->getToken(node);
    if (tok)
    {
        stream << s_root << " [shape=box];" << std::endl;
        String label((const char*) node->getText(node)->chars);
        std::replace(label.begin(), label.end(), '"', '\'');
        stream << s_root << " [label=\"" << label << "\"];" << std::endl;
    }
    else
        stream << s_root << " [shape=circle];" << std::endl;

    uint32 n = node->getChildCount(node);
    for (uint32 i = 0; i < n; ++i)
    {
        pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) node->getChild(node, i);
        StringStream s1;
        s1 << child;

        stream << s_root << " -> " << s1.str().substr(3) << ";" << std::endl;
        dumpNode(child, stream);
    }
}

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

const String& SCsTranslatorFactory::getFileExt() const
{
    static String ext = "scs";
    return ext;
}
