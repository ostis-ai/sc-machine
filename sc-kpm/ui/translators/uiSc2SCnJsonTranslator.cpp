/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "uiPrecompiled.h"
#include "uiSc2SCnJsonTranslator.h"

#include "uiTranslators.h"
#include "uiKeynodes.h"
#include "uiUtils.h"

uiSCnSentenceNode::uiSCnSentenceNode(sScElementInfo *keywordEl)
    : mParent(0)
    , mType(ST_KEYWORD)
    , mElementInfo(keywordEl)
{
}

uiSCnSentenceNode::~uiSCnSentenceNode()
{
    destroyChilds();
}

uiSCnSentenceNode::uiSCnSentenceNode()
    : mParent(0)
    , mType(ST_NODE)
    , mElementInfo(0)
{

}

void uiSCnSentenceNode::buildTree()
{
    mElementInfo->isInSentenceTree = true;

    // iterate all output and input arcs and create child tree nodes
    if (mType != ST_PREDICATE)
    {
        sScElementInfo::tScElementInfoList::iterator it, itEnd = mElementInfo->outputArcs.end();
        for (it = mElementInfo->outputArcs.begin(); it != itEnd; ++it)
        {
            if (!(*it)->isInSentenceTree)
                _createChildPredicate(*it);
        }
        itEnd = mElementInfo->inputArcs.end();
        for (it = mElementInfo->inputArcs.begin(); it != itEnd; ++it)
        {
            if (!(*it)->isInSentenceTree)
                _createChildPredicate(*it);
        }
    }

    if (mType == ST_PREDICATE)
    {
        sScElementInfo *el = (mParent->mElementInfo == mElementInfo->target) ? mElementInfo->source : mElementInfo->target;
        assert(el);

        uiSCnSentenceNode *child = new uiSCnSentenceNode();
        child->mType = ST_NODE;
        child->mElementInfo = el;
        _appendChildNode(child);
    }

    // recursively call this funciton for childs
    tSentenceNodeList::iterator it, itEnd = mChildSentences.end();
    for (it = mChildSentences.begin(); it != itEnd; ++it)
        (*it)->buildTree();
}

void uiSCnSentenceNode::balance()
{

}

const String& uiSCnSentenceNode::json()
{

    StringStream ss;

    ss << "{";
    ss << "\"id\": \"" << uiTranslateFromSc::buildId(mElementInfo->addr) << "\"";
    ss << ", \"type\": " << mElementInfo->type;

    if (mType == ST_KEYWORD)
        ss << ", \"keyword\": true";

    if (mType == ST_PREDICATE)
    {
        assert(mChildSentences.size() == 1);
        ss << ", \"SCNode\" : " << (*mChildSentences.begin())->json();
    } else
    {
        ss << ", \"SCArcs\": [";

        // translate child nodes, they must be predicates
        tSentenceNodeList::iterator it, itEnd = mChildSentences.end();
        for (it = mChildSentences.begin(); it != itEnd; ++it)
        {
            uiSCnSentenceNode *sentence = *it;
            assert(sentence->mType == ST_PREDICATE);

            if (it != mChildSentences.begin())
                ss << ", ";

            ss << sentence->json();
        }
        ss << "]";
    }

    ss << "}";

    mJSONData = ss.str();
    return mJSONData;
}

uiSCnSentenceNode* uiSCnSentenceNode::createChildNode(uiSCnSentenceNode::eSentenceNodeType type)
{
    uiSCnSentenceNode *child = new uiSCnSentenceNode();
    child->mType = type;
    _appendChildNode(child);
    return child;
}

void uiSCnSentenceNode::destroyChilds()
{
    tSentenceNodeList::iterator it, itEnd = mChildSentences.end();
    for (it = mChildSentences.begin(); it != itEnd; ++it)
        delete *it;
    for (it = mContourSentences.begin(); it != mContourSentences.end(); ++it)
        delete *it;
}

void uiSCnSentenceNode::_removeChildNode(uiSCnSentenceNode *child)
{
    assert(child->mParent == this);
    child->mParent = 0;
    tSentenceNodeList::iterator it, itEnd = mChildSentences.end();
    for (it = mChildSentences.begin(); it != itEnd; ++it)
    {
        if (*it == child)
        {
            mChildSentences.erase(it);
            return;
        }
    }
}

void uiSCnSentenceNode::_appendChildNode(uiSCnSentenceNode *child)
{
    assert(child->mParent != this);

    if (child->mParent)
        child->mParent->_removeChildNode(child);

    child->mParent = this;
    mChildSentences.push_back(child);
}

bool uiSCnSentenceNode::_hasChildNode(uiSCnSentenceNode *child) const
{
    tSentenceNodeList::const_iterator it, itEnd = mChildSentences.end();
    for (it = mChildSentences.begin(); it != itEnd; ++it)
    {
        if (*it == child)
            return true;
    }

    return false;
}

void uiSCnSentenceNode::_createChildPredicate(sScElementInfo *arc)
{
    assert(arc->type & sc_type_arc_mask);

    uiSCnSentenceNode *child = createChildNode(ST_PREDICATE);
    child->mElementInfo = arc;
}




// ---------------------------------------------------------------------------------------------------
uiSc2SCnJsonTranslator::uiSc2SCnJsonTranslator()
    : mScElementsInfoPool(0)
{
}

uiSc2SCnJsonTranslator::~uiSc2SCnJsonTranslator()
{
    if (mScElementsInfoPool)
        delete []mScElementsInfoPool;
}

void uiSc2SCnJsonTranslator::runImpl()
{
    // get command arguments
    sc_iterator5 *it5 = sc_iterator5_a_a_f_a_f_new(s_default_ctx,
                                                   sc_type_node | sc_type_const,
                                                   sc_type_arc_common | sc_type_const,
                                                   mInputConstructionAddr,
                                                   sc_type_arc_pos_const_perm,
                                                   keynode_question_nrel_answer);
    if (sc_iterator5_next(it5) == SC_TRUE)
    {
        sc_iterator3 *it3 = sc_iterator3_f_a_a_new(s_default_ctx,
                                                   sc_iterator5_value(it5, 0),
                                                   sc_type_arc_pos_const_perm,
                                                   0);
        while (sc_iterator3_next(it3) == SC_TRUE)
            mKeywordsList.push_back(sc_iterator3_value(it3, 2));
        sc_iterator3_free(it3);
    }
    sc_iterator5_free(it5);

    collectScElementsInfo();



    mOutputData = "[";

    tScAddrList::iterator it, itEnd = mKeywordsList.end();
    for (it = mKeywordsList.begin(); it != itEnd; ++it)
    {
        uiSCnSentenceNode *sentence = new uiSCnSentenceNode(mScElementsInfo[*it]);
        mRootSentences.push_back(sentence);
    }

    // build sentence trees
    uiSCnSentenceNode::tSentenceNodeList::iterator itSentence, itSentenceEnd = mRootSentences.end();
    for (itSentence = mRootSentences.begin(); itSentence != itSentenceEnd; ++itSentence)
        (*itSentence)->buildTree();

    // balance all trees
    for (itSentence = mRootSentences.begin(); itSentence != itSentenceEnd; ++itSentence)
        (*itSentence)->balance();

    // generate json for all trees
    for (itSentence = mRootSentences.begin(); itSentence != itSentenceEnd; ++itSentence)
    {
        if (itSentence != mRootSentences.begin())
            mOutputData += ",";

        mOutputData += (*itSentence)->json();
    }

    // destroy root sentences
    for (itSentence = mRootSentences.begin(); itSentence != itSentenceEnd; ++itSentence)
        delete (*itSentence);
    mRootSentences.clear();

    mOutputData += "]";
    //qDebug() << "Result: " << QString().fromStdString(mOutputData);
}

String uiSc2SCnJsonTranslator::translateElement(sc_addr addr, bool isKeyword)
{

    return "";
}

bool uiSc2SCnJsonTranslator::isInOutputConstruction(sc_addr addr) const
{
    sc_iterator3 *it3 = sc_iterator3_f_a_f_new(s_default_ctx,
                                               mInputConstructionAddr,
                                               sc_type_arc_pos_const_perm,
                                               addr);
    bool result = (sc_iterator3_next(it3) == SC_TRUE);
    sc_iterator3_free(it3);
    return result;
}



void uiSc2SCnJsonTranslator::collectScElementsInfo()
{
    sc_uint32 elementsCount =(sc_uint32) mObjects.size();
    mScElementsInfoPool = new sScElementInfo[elementsCount];

    sc_uint32 poolUsed = 0;
    // first of all collect information about elements
    tScAddrToScTypeMap::const_iterator it, itEnd = mObjects.end();
    for (it = mObjects.begin(); it != itEnd; ++it)
    {
        if (mScElementsInfo.find(it->first) != mScElementsInfo.end())
            continue;

        mScElementsInfoPool[poolUsed].addr = it->first;
        mScElementsInfoPool[poolUsed].type = it->second;
        mScElementsInfo[it->first] = &(mScElementsInfoPool[poolUsed]);
        poolUsed++;
    }

    // now we need to itarete all arcs and collect output/input arcs info
    sc_type elType;
    sc_addr begAddr, endAddr, arcAddr;
    for (it = mObjects.begin(); it != itEnd; ++it)
    {
        arcAddr = it->first;
        elType = it->second;

        sScElementInfo *elInfo = mScElementsInfo[arcAddr];
        elInfo->isInSentenceTree = false;
        elInfo->visualType = sScElementInfo::VT_NODE;

        // skip nodes and links
        if (!(elType & sc_type_arc_mask))
            continue;

        // get begin/end addrs
        if (sc_memory_get_arc_begin(s_default_ctx, arcAddr, &begAddr) != SC_RESULT_OK)
            continue; // @todo process errors
        if (sc_memory_get_arc_end(s_default_ctx, arcAddr, &endAddr) != SC_RESULT_OK)
            continue; // @todo process errors


        elInfo->srcAddr = begAddr;
        elInfo->trgAddr = endAddr;

        sScElementInfo *begInfo = mScElementsInfo[begAddr];
        sScElementInfo *endInfo = mScElementsInfo[endAddr];

        elInfo->source = begInfo;
        elInfo->target = endInfo;

        // check if arc is not broken
        if (begInfo == 0 || endInfo == 0)
            continue;
        endInfo->inputArcs.push_back(mScElementsInfo[arcAddr]);
        begInfo->outputArcs.push_back(mScElementsInfo[arcAddr]);
    }

    // now determine visual type of elements
    tScElemetsInfoMap::iterator itInfo, itInfoEnd = mScElementsInfo.begin();
    for (itInfo = mScElementsInfo.begin(); itInfo != itInfoEnd; ++itInfo)
    {
        sScElementInfo *el = itInfo->second;

        // possible sc-lement can be visualized as set
        if (el->type & sc_type_node_tuple)
        {
            el->visualType = sScElementInfo::VT_SET;
        }

        // possible sc-element can be visualized as contour
        if (el->type & sc_type_node_struct)
        {

        }

    }

    // construct tree of sentences

}

// -------------------------------------
sc_result uiSc2SCnJsonTranslator::ui_translate_sc2scn(const sc_event *event, sc_addr arg)
{
    sc_addr cmd_addr, input_addr, format_addr;

    if (sc_memory_get_arc_end(s_default_ctx, arg, &cmd_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (ui_check_cmd_type(cmd_addr, keynode_command_translate_from_sc) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (ui_translate_command_resolve_arguments(cmd_addr, &format_addr, &input_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (format_addr == keynode_format_scn_json)
    {
        uiSc2SCnJsonTranslator translator;
        translator.translate(input_addr, format_addr);
    }

    return SC_RESULT_OK;
}


