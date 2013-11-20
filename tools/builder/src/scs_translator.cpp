#include "scs_translator.h"
#include "utils.h"

#include <fstream>
#include <iostream>
#include <assert.h>

SCsTranslator::tStringAddrMap SCsTranslator::msGlobalIdtfAddrs = SCsTranslator::tStringAddrMap();
uint32 SCsTranslator::msIdCounter = 1;


#define GET_NODE_TEXT(node) String((const char*)node->getText(node)->chars)

// ------------------------

SCsTranslator::SCsTranslator()
{
}

SCsTranslator::~SCsTranslator()
{
    // destroy element descrptions
    tElementSet::iterator it, itEnd = mElementSet.end();
    for (it = mElementSet.begin(); it != itEnd; ++it)
        delete *it;
    mElementSet.clear();
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

    dumpDot(tree);
    // translate
    buildScText(tree);


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
            processSentenceAssign(sentenceNode);
            break;
        default:
            std::cerr << "Unknown sentence type. Sentence " << i + 1 << std::endl;
            break;
        }
    }


    // now generate sc-text in memory
    std::cout << "Determine element types" << std::endl;
    tElementSet::iterator it, itEnd = mElementSet.end();
    for (it = mElementSet.begin(); it != itEnd; ++it)
    {
        sElement *el = *it;
        assert(el);

        determineElementType(el);

        if (el->type == sc_type_arc_pos_const_perm)
        {
            sc_type type = _getTypeBySetIdtf(el->arc_src->idtf);
            if (type != 0)
            {
                el->ignore = true;
                sc_type newType = el->type | type;
                // TODO check conflicts in sc-type
                if (type & sc_type_constancy_mask != 0)
                    newType = (type & sc_type_constancy_mask) | (newType & ~sc_type_constancy_mask);
                el->type = newType;
            }
        }

        std::cout << el->idtf << ": " << el->type << std::endl;
    }

    dumpScs("test.pscs");

    std::cout << "Generate sc-text" << std::endl;



    return true;
}

SCsTranslator::eSentenceType SCsTranslator::determineSentenceType(pANTLR3_BASE_TREE node)
{
    pANTLR3_COMMON_TOKEN tok = node->getToken(node);
    assert(tok);

    if (tok->type == SEP_SIMPLE)
        return SentenceLevel1;

    if (tok->type == CONNECTORS)
        return SentenceLevel2_7;

    if (tok->type == SEP_ASSIGN)
        return SentenceAssign;

    return SentenceUnknown;
}

#define GENERATE_ATTRS(idx) \
tElementSet::iterator itSubj, itSubjEnd = subjects.end(); \
for (itSubj = subjects.begin(); itSubj != itSubjEnd; ++itSubj) \
{ \
    sElement *el_subj = *itSubj; \
    sElement *el_arc = _addEdge(el_obj, el_subj, type_connector, _isConnectorReversed(connector), ""); \
    tElementSet::iterator itAttrs, itAttrsEnd = var_attrs.end(); \
    for (itAttrs = var_attrs.begin(); itAttrs != itAttrsEnd; ++itAttrs) \
        _addEdge(*itAttrs, el_arc, sc_type_arc_access | sc_type_var | sc_type_arc_pos | sc_type_arc_perm, false, ""); \
    itAttrsEnd = const_attrs.end(); \
    for (itAttrs = const_attrs.begin(); itAttrs != itAttrsEnd; ++itAttrs) \
        _addEdge(*itAttrs, el_arc, sc_type_arc_pos_const_perm, false, ""); \
    if (generate_order) \
    { \
        StringStream ss; \
        ss << (idx) << "'"; \
        _addEdge(_addNode(ss.str()), el_arc, sc_type_arc_pos_const_perm, false, ""); \
    } \
}

void SCsTranslator::processAttrsIdtfList(bool ignore_first, pANTLR3_BASE_TREE node, sElement *el_obj, const String &connector, bool generate_order)
{

    sc_type type_connector = _getTypeByConnector(connector);

    tElementSet var_attrs, const_attrs;
    tElementSet subjects;
    uint32 n = node->getChildCount(node);
    for (uint32 i = ignore_first ? 1 : 0; i < n; ++i)
    {
        pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)node->getChild(node, i);
        pANTLR3_COMMON_TOKEN tok = child->getToken(child);
        assert(tok);
        if (tok->type == SEP_ATTR_CONST || tok->type == SEP_ATTR_VAR)
        {
            if (!subjects.empty())
            {
                GENERATE_ATTRS(i + 1)
                subjects.clear();
                const_attrs.clear();
                var_attrs.clear();
            }
            pANTLR3_BASE_TREE nd = (pANTLR3_BASE_TREE)child->getChild(child, 0);
            sElement *el = _addNode(GET_NODE_TEXT(nd));
            if (tok->type == SEP_ATTR_CONST)
                const_attrs.insert(el);
            else
                var_attrs.insert(el);
        } else
        {
            subjects.insert(parseElementTree(child));
        }
    }
    GENERATE_ATTRS(n)
}

void SCsTranslator::processSentenceLevel1(pANTLR3_BASE_TREE node)
{
    unsigned int nodesCount = node->getChildCount(node);
    assert(nodesCount == 3);

    pANTLR3_BASE_TREE node_obj = (pANTLR3_BASE_TREE)node->getChild(node, 0);
    pANTLR3_BASE_TREE node_pred = (pANTLR3_BASE_TREE)node->getChild(node, 1);
    pANTLR3_BASE_TREE node_subj = (pANTLR3_BASE_TREE)node->getChild(node, 2);

    pANTLR3_COMMON_TOKEN tok_pred = node_pred->getToken(node_pred);

    if (tok_pred->type != ID_SYSTEM)
        std::cerr << "Invalid predicate '" << ((const char*) node_pred->getText(node_pred)->chars) << "' in simple sentence" << std::endl;

    sElement *el_obj = parseElementTree(node_obj);
    sElement *el_subj = parseElementTree(node_subj);

    // determine arc type
    sc_type type = sc_type_edge_common;
    String pred = GET_NODE_TEXT(node_pred);
    size_t n = pred.find_first_of("#");
    if (n != pred.npos)
        type = _getArcPreffixType(pred.substr(0, n));

    _addEdge(el_obj, el_subj, type, false, pred);
}

void SCsTranslator::processSentenceLevel2_7(pANTLR3_BASE_TREE node)
{
    String connector = GET_NODE_TEXT(node);


    // determine object
    pANTLR3_BASE_TREE node_obj = (pANTLR3_BASE_TREE)node->getChild(node, 0);
    sElement *el_obj = _createElement(GET_NODE_TEXT(node_obj));

    // no we need to parse attributes and predicates
    processAttrsIdtfList(true, node, el_obj, connector, false);
}

void SCsTranslator::processSentenceAssign(pANTLR3_BASE_TREE node)
{
    unsigned int nodesCount = node->getChildCount(node);
    assert(nodesCount == 2);

    pANTLR3_BASE_TREE node_left = (pANTLR3_BASE_TREE)node->getChild(node, 0);
    pANTLR3_BASE_TREE node_right = (pANTLR3_BASE_TREE)node->getChild(node, 1);

    mAssignments[GET_NODE_TEXT(node_left)] = GET_NODE_TEXT(node_right);
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

    SC_ADDR_MAKE_EMPTY(addr);

    return addr;
}

sc_addr SCsTranslator::createScAddr(sElement *el)
{
    sc_addr addr;

    if (el->type & sc_type_node)
        addr = sc_memory_node_new(el->type);
    else if (el->type & sc_type_link)
        addr = sc_memory_link_new();
    else
    {
        assert(el->arc_src && el->arc_trg);
        assert(SC_ADDR_IS_NOT_EMPTY(el->arc_src->addr) && SC_ADDR_IS_NOT_EMPTY(el->arc_trg->addr));
        addr = sc_memory_arc_new(el->type, el->arc_src->addr, el->arc_trg->addr);
    }

    el->addr = addr;

    // store in addrs map
    if (!el->idtf.empty())
    {
        if (StringUtil::startsWith(el->idtf, "..", false))
            mLocalIdtfAddrs[el->idtf] = addr;
        else if (StringUtil::startsWith(el->idtf, ".", false))
            msGlobalIdtfAddrs[el->idtf] = addr;
        else
            mSysIdtfAddrs[el->idtf] = addr;
    }

    return addr;
}

void SCsTranslator::determineElementType(sElement *el)
{
    assert(el);
    sc_type oldType = el->type;
    sc_type newType = oldType;

    newType = (newType & (~sc_type_constancy_mask)) | (StringUtil::startsWith(el->idtf, "_", false) ? sc_type_var : sc_type_const);

    el->type = newType;
}

sElement* SCsTranslator::_createElement(const String &idtf)
{
    if (!idtf.empty())
    {
        tElementIdtfMap::iterator it = mElementIdtf.find(idtf);
        if (it != mElementIdtf.end())
            return it->second;
    }

    sElement *el = new sElement();

    el->id = msIdCounter++;
    el->idtf = idtf;
    assert(mElementIdtf.find(idtf) == mElementIdtf.end());
    if (!idtf.empty())
        mElementIdtf[idtf] = el;
    mElementSet.insert(el);

    return el;
}

sElement* SCsTranslator::_addNode(const String &idtf)
{
    sElement *el = _createElement(idtf);

    el->type = sc_type_node;

    return el;
}

sElement* SCsTranslator::_addEdge(sElement *source, sElement *target, sc_type type, bool is_reversed, const String &idtf)
{
    assert(source && target);

    sElement *el = _createElement(idtf);

    el->type = type;
    if (is_reversed)
    {
        el->arc_src = target;
        el->arc_trg = source;
    } else
    {
        el->arc_src = source;
        el->arc_trg = target;
    }

    return el;
}

sElement* SCsTranslator::_addLink(bool is_file, const String &data)
{
    sElement *el = _createElement(is_file ? data : "");

    el->type = sc_type_link;
    el->link_is_file = is_file;
    el->link_data = data;

    return el;
}

sElement* SCsTranslator::parseElementTree(pANTLR3_BASE_TREE tree)
{
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    assert(tok);

    sElement *res = 0;
    if (tok->type == ID_SYSTEM)
        res = _addNode(GET_NODE_TEXT(tree));

    if (tok->type == SEP_LPAR)
    {
        assert(tree->getChildCount(tree) >= 3);
        pANTLR3_BASE_TREE node_obj = (pANTLR3_BASE_TREE)tree->getChild(tree, 0);
        pANTLR3_BASE_TREE node_pred = (pANTLR3_BASE_TREE)tree->getChild(tree, 1);
        pANTLR3_BASE_TREE node_subj = (pANTLR3_BASE_TREE)tree->getChild(tree, 2);

        String pred = GET_NODE_TEXT(node_pred);
        sElement *src = parseElementTree(node_obj);
        sElement *trg = parseElementTree(node_subj);

        assert(src && trg);

        res = _addEdge(src, trg, _getTypeByConnector(pred), _isConnectorReversed(pred), "");
    }

    if (tok->type == LINK)
        res = _addLink(true, GET_NODE_TEXT(tree));

    if (tok->type == SEP_LTUPLE || tok->type == SEP_LSET)
    {

    }

    // now process internal sentences
    uint32 n = tree->getChildCount(tree);
    for (uint32 i = 0; i < n; ++i)
    {
        pANTLR3_BASE_TREE internal = (pANTLR3_BASE_TREE)tree->getChild(tree, i);
        pANTLR3_COMMON_TOKEN tok = internal->getToken(internal);

        if (tok->type != SEP_LINT) continue;

        // process internal sentences
        uint32 nc = internal->getChildCount(internal);
        for (uint32 j = 0; j < nc; ++j)
        {
            pANTLR3_BASE_TREE node_pred = (pANTLR3_BASE_TREE)internal->getChild(internal, j);
            String connector = GET_NODE_TEXT(node_pred);
            processAttrsIdtfList(false, node_pred, res, connector, false);
        }
    }

    return res;
}

sc_type SCsTranslator::_getArcPreffixType(const String &preffix) const
{
    // do not use map, to prevent it initialization on class creation
    if (preffix == "sc_arc_common")
        return sc_type_arc_common;

    if (preffix == "sc_arc_main")
        return sc_type_arc_pos_const_perm;

    if (preffix == "sc_arc_access")
        return sc_type_arc_access;

    return sc_type_edge_common;
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

sc_type SCsTranslator::_getTypeByConnector(const String &connector)
{
    if (connector == ">" || connector == "<")
        return sc_type_arc_common;
    if (connector == "->" || connector == "<-")
        return sc_type_arc_pos_const_perm;
    if (connector == "<>")
        return sc_type_edge_common;
    if (connector == "..>" || connector == "<..")
        return sc_type_arc_access;
    if (connector == "<=>")
        return sc_type_edge_common | sc_type_const;
    if (connector == "_<=>")
        return sc_type_edge_common | sc_type_var;

    if (connector == "=>" || connector == "<=")
        return sc_type_arc_common | sc_type_const;
    if (connector == "_=>" || connector == "_<=")
        return sc_type_arc_common | sc_type_var;
    if (connector == "_->" || connector == "_<-")
        return sc_type_arc_access | sc_type_arc_pos | sc_type_var | sc_type_arc_perm;
    if (connector == "-|>" || connector == "<|-")
        return sc_type_arc_access | sc_type_arc_neg | sc_type_const | sc_type_arc_perm;
    if (connector == "_-|>" || connector == "_<|-")
        return sc_type_arc_access | sc_type_arc_neg | sc_type_var | sc_type_arc_perm;
    if (connector == "-/>" || connector == "</-")
        return sc_type_arc_access | sc_type_arc_fuz | sc_type_const | sc_type_arc_perm;
    if (connector == "_-/>" || connector == "_</-")
        return sc_type_arc_access | sc_type_arc_fuz | sc_type_var | sc_type_arc_perm;
    if (connector == "~>" || connector == "<~")
        return sc_type_arc_access | sc_type_arc_pos | sc_type_const | sc_type_arc_temp;
    if (connector == "_~>" || connector == "_<~")
        return sc_type_arc_access | sc_type_arc_pos | sc_type_var | sc_type_arc_temp;
    if (connector == "~|>" || connector == "<|~")
        return sc_type_arc_access | sc_type_arc_neg | sc_type_const | sc_type_arc_temp;
    if (connector == "_~|>" || connector == "_<|~")
        return sc_type_arc_access | sc_type_arc_neg | sc_type_var | sc_type_arc_temp;
    if (connector == "~/>" || connector == "</~")
        return sc_type_arc_access | sc_type_arc_fuz | sc_type_const | sc_type_arc_temp;
    if (connector == "_~/>" || connector == "_</~")
        return sc_type_arc_access | sc_type_arc_fuz | sc_type_var | sc_type_arc_temp;

    return 0;
}

bool SCsTranslator::_isConnectorReversed(const String &connector)
{
    if (connector == "<" || connector == "<-" || connector == "<.." || connector == "<=" ||
        connector == "_<=" || connector == "_<-" || connector == "<|-" || connector == "_<|-" ||
        connector == "</-" || connector == "_</-" || connector == "<~" || connector == "_<~" ||
        connector == "<|~" || connector == "_<|~" || connector == "</~" || connector == "_</~")
        return true;

    return false;
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

void SCsTranslator::dumpScs(const String &fileName)
{
    std::ofstream out(fileName.c_str());

    tElementSet::iterator it, itEnd = mElementSet.end();
    for (it = mElementSet.begin(); it != itEnd; ++it)
    {
        sElement *el = *it;

        StringStream s1, s2, s3;
        s1 << el->arc_src;
        s2 << el;
        s3 << el->arc_trg;


        if (el->type & sc_type_arc_mask)
            out << (el->arc_src->idtf.empty() ? s1.str() : el->arc_src->idtf) << " | " << (el->idtf.empty() ? s2.str() : el->idtf) << " | " << (el->arc_trg->idtf.empty() ? s3.str() : el->arc_trg->idtf) << ";" << std::endl;
    }

    out.close();
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
