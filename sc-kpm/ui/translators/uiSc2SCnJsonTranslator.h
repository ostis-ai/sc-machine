/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _uiSc2SCnJsonTranslator_h_
#define _uiSc2SCnJsonTranslator_h_

#include "uiTranslatorFromSc.h"

struct sScElementInfo
{
    typedef std::list<sScElementInfo*> tScElementInfoList;

    typedef enum
    {
        VT_NODE,
        VT_EDGE,
        VT_SET,
        VT_CONTOUR

    } eVisualType;

    sc_type type;
    sc_addr addr;
    sc_addr srcAddr;
    sc_addr trgAddr;
    sScElementInfo* source;
    sScElementInfo* target;
    tScElementInfoList outputArcs;
    tScElementInfoList inputArcs;

    eVisualType visualType;
    bool isInSentenceTree;
};

class uiSCnSentenceNode
{
    typedef enum
    {
        ST_KEYWORD,
        ST_PREDICATE,
        ST_NODE
    } eSentenceNodeType;

public:
    typedef std::list<uiSCnSentenceNode*> tSentenceNodeList;

    //! Create keyword sentence node
    explicit uiSCnSentenceNode(sScElementInfo *keywordEl);
    virtual ~uiSCnSentenceNode();

    //! Build tree based on keyword. This node must be a keyword
    void buildTree();
    //! Balance tree
    void balance();
    //! Generates json for specified tree
    const String& json();

    //! Append new child tree node to this one
    uiSCnSentenceNode* createChildNode(eSentenceNodeType type);

protected:
    //! Destroy tree recursively
    void destroyChilds();

    //! Removes node from childs list
    void _removeChildNode(uiSCnSentenceNode *child);
    //! Appends new node to childs list
    void _appendChildNode(uiSCnSentenceNode *child);
    //! Check if specified tree node exists in child list of this one
    bool _hasChildNode(uiSCnSentenceNode *child) const;


private:
    explicit uiSCnSentenceNode();

    //! Create child predicate node
    void _createChildPredicate(sScElementInfo *arc);

private:
    //! Pointer to parent sentence tree node
    uiSCnSentenceNode *mParent;
    //! Type of sentence tree node
    eSentenceNodeType mType;
    //! List of conotur article root nodes
    tSentenceNodeList mContourSentences;
    //! List of child set sentences
    tSentenceNodeList mChildSetSentences;
    //! List of child sentences
    tSentenceNodeList mChildSentences;
    //! Pointer to sc-element info
    sScElementInfo *mElementInfo;
    //! String that contains cache of generated json for tree
    String mJSONData;
};


/*!
 * \brief Class that translates sc-construction into
 * SCn-code (json representation)
 */
class uiSc2SCnJsonTranslator : public uiTranslateFromSc
{
public:
    explicit uiSc2SCnJsonTranslator();
    virtual ~uiSc2SCnJsonTranslator();


    static sc_result ui_translate_sc2scn(const sc_event *event, sc_addr arg);

protected:
    //! @copydoc uiTranslateFromSc::runImpl
    void runImpl();

    /*! Translate one sc-element semantic neighborhood
     * @param addr sc-addr of sc-element to translate
     * @param isKeyword Keyword flag
     * @return Returns string that json representaion of sc-element
     */
    String translateElement(sc_addr addr, bool isKeyword);

    //! Check if specified sc-element included in output construction
    bool isInOutputConstruction(sc_addr addr) const;

    //! Collect information of trsnslated sc-elements and store it
    void collectScElementsInfo();

protected:
    //! List of keywords
    tScAddrList mKeywordsList;
    //! Collection of objects information
    typedef std::map<sc_addr, sScElementInfo*> tScElemetsInfoMap;
    tScElemetsInfoMap mScElementsInfo;
    //! Pull of sc-elements information (used to prevent many memory allocations)
    sScElementInfo *mScElementsInfoPool;
    //! List of articles root elements
    uiSCnSentenceNode::tSentenceNodeList mRootSentences;
};



#endif // _uiSc2SCnJsonTranslator_h_
