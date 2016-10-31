/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "scs_translator.h"
#include "utils.h"

#include <fstream>
#include <iostream>
#include <assert.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>




#define GET_NODE_TEXT(node) String((const char*)node->getText(node)->chars)

long long SCsTranslator::msAutoIdtfCount = 0;

// ------------------------

String trimContentData(String const & path)
{
	if (path.size() < 2)
		return String();

	return path.substr(2, path.size() - 3);
}

SCsTranslator::SCsTranslator(sc_memory_context *ctx)
    : iTranslator(ctx)
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

bool SCsTranslator::translateImpl()
{
    // open file and read data
    bool result = true;
    std::ifstream ifs(mParams.fileName.c_str());
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
    return SCsTranslatorFactory::EXTENSION;
}

bool SCsTranslator::processString(const String &data)
{
    pANTLR3_INPUT_STREAM input;

#if defined( __WIN32__ ) || defined( _WIN32 )
    input = antlr3StringStreamNew((pANTLR3_UINT8)data.c_str(), ANTLR3_ENC_UTF8, (ANTLR3_UINT32)data.length(), (pANTLR3_UINT8)"scs");
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


    scsParser_syntax_return r;
    pANTLR3_BASE_TREE tree;

    try
    {
        r = parser->syntax(parser);
    } catch (const Exception &e)
    {
        THROW_EXCEPT(Exception::ERR_PARSE, e.getDescription(), mParams.fileName, e.getLineNumber());
    }

    tree = r.tree;

    //dumpDot(tree);

    // translate
    buildScText(tree);

    //dumpScs("test.scsd");

    parser->free(parser);
    tokens->free(tokens);
    lex->free(lex);

    input->close(input);

    return true;
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
        case SentenceEOF:
            break;
        default:
            THROW_EXCEPT(Exception::ERR_PARSE,
                        "Unknown sentence type.",
                         mParams.fileName,
                         sentenceNode->getLine(sentenceNode));
            break;
        }
    }

    // now generate sc-text in memory
    tElementSet::iterator it, itEnd = mElementSet.end();
    for (it = mElementSet.begin(); it != itEnd; ++it)
    {
        sElement *el = *it;
        assert(el);

        if (el->type == sc_type_arc_pos_const_perm)
        {
            sc_type type = _getTypeBySetIdtf(el->arc_src->idtf);
            if (type != 0)
            {
                el->ignore = true;
				el->arc_src->ignore = true;

                sc_type newType = el->arc_trg->type | type;
                // TODO check conflicts in sc-type
                if ((type & sc_type_constancy_mask) != 0)
                    newType = (type & sc_type_constancy_mask) | (newType & ~sc_type_constancy_mask);
                el->arc_trg->type = newType;
            }
        }

        // arcs already have types
        if (!(el->type & sc_type_arc_mask))
            determineElementType(el);
    }

    tElementSet arcs;
    for (it = mElementSet.begin(); it != itEnd; ++it)
    {
        sElement *el = *it;
        assert(el);

        // skip processed triples
        if (el->ignore) continue;

        sc_addr addr = resolveScAddr(el);

        if (SC_ADDR_IS_EMPTY(addr))
        {
            assert(el->type & sc_type_arc_mask);
            arcs.insert(el);
        }
    }

    bool created = true;
    while (!arcs.empty() && created)
    {
        created = false;

        tElementSet createdSet;
        itEnd = arcs.end();
        for (it = arcs.begin(); it != itEnd; ++it)
        {
            sElement *arc_el = *it;
            assert(arc_el->type & sc_type_arc_mask);
            sc_addr addr = resolveScAddr(arc_el);

            if (SC_ADDR_IS_EMPTY(addr)) continue;

            createdSet.insert(arc_el);
        }

        created = !createdSet.empty();
        itEnd = createdSet.end();
        for (it = createdSet.begin(); it != itEnd; ++it)
            arcs.erase(*it);
    }

    if (!arcs.empty())
    {
        StringStream ss;
        ss << "Arcs not created: " << arcs.size();
        THROW_EXCEPT(Exception::ERR_INVALID_STATE,
                     ss.str(),
                     mParams.fileName,
                     -1);
    }

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

    if (tok->type == EOF)
        return SentenceEOF;

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
        ss << "rrel_" << (idx++); \
        _addEdge(_addNode(ss.str(), sc_type_node_role), el_arc, sc_type_arc_pos_const_perm, false, ""); \
    } \
}

void SCsTranslator::processAttrsIdtfList(bool ignore_first, pANTLR3_BASE_TREE node, sElement *el_obj, const String &connector, bool generate_order)
{

    sc_type type_connector = _getTypeByConnector(connector);

    tElementSet var_attrs, const_attrs;
    tElementSet subjects;
    uint32 n = node->getChildCount(node);
    uint32 idx = 1;
    for (uint32 i = ignore_first ? 1 : 0; i < n; ++i)
    {
        pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)node->getChild(node, i);
        pANTLR3_COMMON_TOKEN tok = child->getToken(child);
        assert(tok);

        // skip internal sentences
        if (tok->type == SEP_LINT) continue;

        if (tok->type == SEP_ATTR_CONST || tok->type == SEP_ATTR_VAR)
        {
            if (!subjects.empty())
            {
                GENERATE_ATTRS(idx)
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
    GENERATE_ATTRS(idx)
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
    {
        THROW_EXCEPT(Exception::ERR_PARSE,
                     String("Invalid predicate '") + ((const char*) node_pred->getText(node_pred)->chars) + "' in simple sentence",
                     mParams.fileName,
                     tok_pred->getLine(tok_pred));
    }

    sElement *el_obj = parseElementTree(node_obj);
    sElement *el_subj = parseElementTree(node_subj);

    // determine arc type
    sc_type type = sc_type_edge_common;
    String pred = GET_NODE_TEXT(node_pred);
    size_t n = pred.find_first_of("#");
    if (n != pred.npos)
        type = _getArcPreffixType(pred.substr(0, n));

    _addEdge(el_obj, el_subj, type, false, pred.substr(n + 1, pred.size() - n - 1));
}

void SCsTranslator::processSentenceLevel2_7(pANTLR3_BASE_TREE node)
{
    String connector = GET_NODE_TEXT(node);


    // determine object
    pANTLR3_BASE_TREE node_obj = (pANTLR3_BASE_TREE)node->getChild(node, 0);
    sElement *el_obj = _createElement(GET_NODE_TEXT(node_obj), 0);

    // no we need to parse attributes and predicates
    processAttrsIdtfList(true, node, el_obj, connector, false);
}

void SCsTranslator::processSentenceAssign(pANTLR3_BASE_TREE node)
{
    unsigned int nodesCount = node->getChildCount(node);
    assert(nodesCount == 2);

    pANTLR3_BASE_TREE node_left = (pANTLR3_BASE_TREE)node->getChild(node, 0);
    pANTLR3_BASE_TREE node_right = (pANTLR3_BASE_TREE)node->getChild(node, 1);

    pANTLR3_COMMON_TOKEN tok_left = node_left->getToken(node_left);
    pANTLR3_COMMON_TOKEN tok_right = node_left->getToken(node_right);

    assert(tok_left && tok_right);

    if (tok_left->type != ID_SYSTEM)
    {
        THROW_EXCEPT(Exception::ERR_PARSE,
                     "Unsupported type of tokens at the left side of assignment sentence",
                     mParams.fileName,
                     tok_left->getLine(tok_left));
    }

    if (tok_right->type == ID_SYSTEM)
    {
        mAssignments[GET_NODE_TEXT(node_left)] = GET_NODE_TEXT(node_right);
    }
    else
    {
        String left_idtf = (GET_NODE_TEXT(node_left));
        sElement *el = parseElementTree(node_right, &left_idtf);
        assert(el != nullptr);
    }
}

sc_addr SCsTranslator::resolveScAddr(sElement *el)
{
    assert(SC_ADDR_IS_EMPTY(el->addr));

    sc_addr addr;
    SC_ADDR_MAKE_EMPTY(addr);
    if (!el->idtf.empty())
    {
        // try to find in system identifiers
        tStringAddrMap::iterator it = mSysIdtfAddrs.find(el->idtf);
        if (it != mSysIdtfAddrs.end())
        {
            addr = it->second;
        } else
        {
            // try to find in global identifiers
            it = msGlobalIdtfAddrs.find(el->idtf);
            if (it != msGlobalIdtfAddrs.end())
                addr = it->second;
            else
            {
                // try to find in local identifiers
                it = mLocalIdtfAddrs.find(el->idtf);
                if (it != mLocalIdtfAddrs.end())
                    addr = it->second;
                else
                {
                    // resolve system identifier
                    sc_result res = sc_helper_find_element_by_system_identifier(mContext, el->idtf.c_str(), (sc_uint32)el->idtf.size(), &addr);
                    if (res == SC_RESULT_OK)
                        mSysIdtfAddrs[el->idtf] = addr;
                }
            }
        }
    }

    if (SC_ADDR_IS_NOT_EMPTY(addr))
    {
        sc_type t = 0;
        if (sc_memory_get_element_type(mContext, addr, &t) == SC_RESULT_OK)
            sc_memory_change_element_subtype(mContext, addr, ~sc_type_element_mask & (el->type | t));

        el->addr = addr;
        return addr;
    }

    // generate addr
    addr = createScAddr(el);

    // store in addrs map
    if (!el->idtf.empty())
    {
        switch (_getIdentifierVisibility(el->idtf))
        {
        case IdtfSystem:
            sc_helper_set_system_identifier(mContext, addr, el->idtf.c_str(), (sc_uint32)el->idtf.size());
            mSysIdtfAddrs[el->idtf] = addr;
            break;
        case IdtfLocal:
            mLocalIdtfAddrs[el->idtf] = addr;
            break;
        case IdtfGlobal:
            msGlobalIdtfAddrs[el->idtf] = addr;
            break;
        }

    }

    return addr;
}

sc_addr SCsTranslator::createScAddr(sElement *el)
{
    sc_addr addr;
    SC_ADDR_MAKE_EMPTY(addr);

    if (el->type & sc_type_node)
    {
        addr = sc_memory_node_new(mContext, el->type);
    }
    else if (el->type & sc_type_link)
    {
        addr = sc_memory_link_new(mContext);

        // setup link content
        if (el->link_is_file)
        {
            String file_path;
            if (_getAbsFilePath(el->file_path, file_path))
            {
                sc_stream *stream = sc_stream_file_new(file_path.c_str(), SC_STREAM_FLAG_READ);
                if (stream)
                {
                    sc_memory_set_link_content(mContext, addr, stream);
                    sc_stream_free(stream);
                } else
                {
                    THROW_EXCEPT(Exception::ERR_FILE_NOT_FOUND,
                                 "Can't open file " + el->file_path,
                                 mParams.fileName,
                                 -1);
                }
            } else
            {
                THROW_EXCEPT(Exception::ERR_INVALID_PARAMS,
                             "Unsupported link type  " + el->file_path,
                             mParams.fileName,
                             -1);
            }

        } else
        {           
            sc_stream *stream = sc_stream_memory_new(el->link_data.data.data(), (sc_uint)el->link_data.data.size(), SC_STREAM_FLAG_READ, SC_FALSE);
            sc_memory_set_link_content(mContext, addr, stream);
            sc_stream_free(stream);
        }


        // generate format information
        if (mParams.autoFormatInfo)
        {
            if (el->link_is_file)
            {
				size_t n = el->file_path.find_last_of(".");
                if (n != String::npos)
					generateFormatInfo(addr, el->file_path.substr(n + 1));
            }
        }
    }
    else
    {
        assert(el->arc_src && el->arc_trg);
        if (SC_ADDR_IS_EMPTY(el->arc_src->addr) || SC_ADDR_IS_EMPTY(el->arc_trg->addr))
            return addr;
        addr = sc_memory_arc_new(mContext, el->type, el->arc_src->addr, el->arc_trg->addr);
    }

    el->addr = addr;

    return addr;
}

void SCsTranslator::determineElementType(sElement *el)
{
    assert(el);

    sc_type oldType = el->type;
    sc_type newType = oldType;

    if ((newType & sc_type_element_mask) == 0)
        newType = newType | sc_type_node;

    newType = (newType & (~sc_type_constancy_mask));
    sc_type const_type = _isIdentifierVar(el->idtf) ? sc_type_var : sc_type_const;
    newType = newType | (const_type);

    el->type = newType;
}

sElement* SCsTranslator::_createElement(const String &idtf, sc_type type)
{
    String newIdtf = idtf;
    if (!idtf.empty())
    {
        if (idtf == "...")
        {
            StringStream ss;
            ss << "..." << msAutoIdtfCount++ << "...auto...";
            newIdtf = ss.str();
        } else {
            tElementIdtfMap::iterator it = mElementIdtf.find(idtf);
            if (it != mElementIdtf.end())
            {
                it->second->type = it->second->type | type;
                return it->second;
            }
        }
    }

    sElement *el = new sElement();

    el->idtf = newIdtf;
    el->type = type;
    assert(mElementIdtf.find(newIdtf) == mElementIdtf.end());
    if (!newIdtf.empty())
        mElementIdtf[newIdtf] = el;
    mElementSet.insert(el);

    return el;
}

sElement* SCsTranslator::_addNode(const String &idtf, sc_type type)
{
    return _createElement(idtf, sc_type_node | type);
}

sElement* SCsTranslator::_addEdge(sElement *source, sElement *target, sc_type type, bool is_reversed, const String &idtf)
{
    assert(source && target);

    sElement *el = _createElement(idtf, type);

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

sElement* SCsTranslator::_addLink(const String &idtf, const sBuffer & data)
{
    sElement *el = _createElement(idtf, sc_type_link);

    el->link_is_file = false;
    el->link_data = data;

    return el;
}

sElement* SCsTranslator::_addLinkFile(const String & idtf, const String & filePath)
{
	sElement *el = _createElement(idtf, sc_type_link);

	el->link_is_file = true;
	el->file_path = filePath;

	return el;
}

sElement* SCsTranslator::_addLinkString(const String & idtf, const String & str)
{
	sElement *el = _createElement(idtf, sc_type_link);

	el->link_is_file = false;
	el->link_data = sBuffer(str.c_str(), (sc_uint)str.size());

	return el;
}

#define CHECK_LINK_DATA(__d) if (__d.empty()) { THROW_EXCEPT(Exception::ERR_PARSE, "Empty link content", mParams.fileName, tok->getLine(tok)) }

sElement* SCsTranslator::parseElementTree(pANTLR3_BASE_TREE tree, const String *assignIdtf)
{
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    assert(tok);

    sElement *res = 0;
    if (tok->type == ID_SYSTEM)
    {
        res = _addNode(GET_NODE_TEXT(tree));
    }

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
    {
        String data = GET_NODE_TEXT(tree);
        CHECK_LINK_DATA(data);
		res = _addLinkFile(assignIdtf ? *assignIdtf : "", data.substr(1, data.size() - 2));
    }

    if (tok->type == CONTENT)
    {
        res = _addNode(assignIdtf ? *assignIdtf : "", sc_type_node_struct);

        String content = GET_NODE_TEXT(tree);
		bool isVar = StringUtil::startsWith(content, "_", false);
        content = content.substr(isVar ? 2 : 1, content.size() - (isVar ? 3 : 2));

        if (StringUtil::startsWith(content, "*", false) && StringUtil::endsWith(content, "*", false))
        {
            // parse contour data
            String data = content.substr(1, content.size() - 2);
            bool autoFormatInfo = mParams.autoFormatInfo;
            String fileName = mParams.fileName;

            // check if link to file
            if (StringUtil::startsWith(data, "^\"", false))
            {
                String name;
				if (_getAbsFilePath(trimContentData(data), name))
                {
                    fileName = name;
                    std::ifstream ifs(name.c_str());
                    if (ifs.is_open())
                    {
                        data = String((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                        ifs.close();
                    }
                    else
                    {
                        THROW_EXCEPT(Exception::ERR_PARSE,
                                     "Can't open file " << name,
                                     mParams.fileName,
                                     tok->getLine(tok));
                    }
                }
            }

            // parse data
            if (!data.empty())
            {
                SCsTranslator translator(mContext);
                translator.mParams.autoFormatInfo = autoFormatInfo;
                translator.mParams.fileName = fileName;
                translator.processString(data);

                // now we need to get all created elements and create arcs to them
                tElementSet::iterator it, itEnd = translator.mElementSet.end();
                for (it = translator.mElementSet.begin(); it != itEnd; ++it)
                {
                    if ((*it)->ignore)
						continue;

                    sElement *el = new sElement();
                    el->ignore = true;
                    el->addr = (*it)->addr;

                    mElementSet.insert(el);
					_addEdge(res, el, isVar ? sc_type_arc_pos_var_perm : sc_type_arc_pos_const_perm, false, "");
                }

                // merge identifiers map
                mSysIdtfAddrs.insert(translator.mSysIdtfAddrs.begin(), translator.mSysIdtfAddrs.end());
                mLocalIdtfAddrs.insert(translator.mLocalIdtfAddrs.begin(), translator.mLocalIdtfAddrs.end());
            }


        }
		else
        {
            if (StringUtil::startsWith(content, "^\"", false))
            {
				String data = trimContentData(content);
                CHECK_LINK_DATA(data);
				sBuffer buffer;
								
				if (parseContentBinaryData(data, buffer))
				{
					res = _addLink("", buffer);
				}
				else
				{
					res = _addLinkFile("", data);
				}
            }
            else
            {
                CHECK_LINK_DATA(content);
                res = _addLinkString("", content);
            }
        }
    }

    if (tok->type == SEP_LTUPLE || tok->type == SEP_LSET)
    {
        res = _addNode("", sc_type_node_tuple);
        processAttrsIdtfList(false, tree, res, "->", tok->type == SEP_LTUPLE);
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

bool SCsTranslator::_getAbsFilePath(const String &url, String &abs_path)
{
    // split file name
    String path, name;
    StringUtil::splitFilename(mParams.fileName, name, path);
    static String file_preffix = "file://";
    if (StringUtil::startsWith(url, file_preffix, true))
    {
        String file_path = url.substr(file_preffix.size());
        if (!StringUtil::startsWith(file_path, "/", false))
        {
            boost::filesystem::path dir (path);
            boost::filesystem::path file (file_path);
            boost::filesystem::path full_path = dir / file;
            abs_path = full_path.string();
        }

        return true;
    }

    return false;

}

bool SCsTranslator::_isIdentifierVar(const String &idtf) const
{
    // remove visibility points
    String s = idtf;
    int i = 0;
    while ((i < 2) && StringUtil::startsWith(s, ".", false))
    {
        s = s.substr(1);
        ++i;
    }

    return StringUtil::startsWith(s, "_", false);
}

bool SCsTranslator::parseContentBinaryData(String const & data, sBuffer & outBuffer)
{
	std::string const valueStr = data.substr(6);

	if (StringUtil::startsWith(data, "float", true))
	{
		float value = (float)::atof(valueStr.c_str());
		outBuffer.set((char*)&value, sizeof(value));
		return true;
	}

	if (StringUtil::startsWith(data, "double", true))
	{
		double value = (double)::atof(valueStr.c_str());
		outBuffer.set((char*)&value, sizeof(value));
		return true;
	}

	if (StringUtil::startsWith(data, "int64", true))
	{
		sc_int64 value = (sc_int64)::atoll(valueStr.c_str());
		outBuffer.set((char*)&value, sizeof(value));
		return true;
	}

	if (StringUtil::startsWith(data, "int32", true))
	{
		sc_int32 value = (sc_int32)::atoi(valueStr.c_str());
		outBuffer.set((char*)&value, sizeof(value));
		return true;
	}

	if (StringUtil::startsWith(data, "int16", true))
	{
		sc_int16 value = (sc_int16)::atoi(valueStr.c_str());
		outBuffer.set((char*)&value, sizeof(value));
		return true;
	}

	if (StringUtil::startsWith(data, "int8", true))
	{
		sc_int8 value = (sc_int8)::atoi(valueStr.c_str());
		outBuffer.set((char*)&value, sizeof(value));
		return true;
	}

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
            out << (el->arc_src->idtf.empty() ? s1.str() : el->arc_src->idtf) << "." << el->arc_src->type << " | " <<
                   (el->idtf.empty() ? s2.str() : el->idtf) << "." << el->type << " | " <<
                   (el->arc_trg->idtf.empty() ? s3.str() : el->arc_trg->idtf) << "." << el->arc_trg->type << ";" << std::endl;
    }

    out.close();
}

// -------------
const String SCsTranslatorFactory::EXTENSION = "scs";

SCsTranslatorFactory::SCsTranslatorFactory()
{

}

SCsTranslatorFactory::~SCsTranslatorFactory()
{

}

iTranslator* SCsTranslatorFactory::createInstance(sc_memory_context *ctx)
{
    return new SCsTranslator(ctx);
}

const String& SCsTranslatorFactory::getFileExt() const
{
    return EXTENSION;
}
