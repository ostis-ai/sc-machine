#ifndef _scs_translator_h_
#define _scs_translator_h_

#include "translator.h"

#include "parser/scsLexer.h"
#include "parser/scsParser.h"

struct sc_add_compare
{
    bool operator() (const sc_addr &addr1, const sc_addr &addr2) const
    {
        if (addr1.seg < addr2.seg) return true;
        if (addr1.seg > addr2.seg) return false;
        if (addr1.offset < addr2.offset) return true;
        return false;
    }
};

struct sElement
{
    uint32 id;
    sc_type type;
    sc_addr addr;
    String idtf;

    // arc info
    sElement *arc_src;
    sElement *arc_trg;

    // link info
    bool link_is_file;
    String link_data;

    bool ignore;

    sElement()
        : id(0)
        , type(0)
        , arc_src(0)
        , arc_trg(0)
        , link_is_file(false)
        , ignore(false)
    {
        SC_ADDR_MAKE_EMPTY(addr);
    }
};

// ---------------------------------------------------------

class SCsTranslator : public iTranslator
{
    typedef enum
    {
        SentenceUnknown = 0,
        SentenceLevel1 = 1,
        SentenceLevel2_7 = 2,
        SentenceAssign = 3
    } eSentenceType;

public:
    explicit SCsTranslator();
    virtual ~SCsTranslator();

    //! @copydoc iTranslator::translate
    bool translate(const String &filename);
    //! @copydoc iTranlstor::getFileExt
    const String& getFileExt() const;

private:
    //! Process string data
    bool processString(const String &data);
    /*! Builds sc-text based on parsed antlr tree
     * @param tree Pointer to root antlr tree node
     * @returns If tree parsed without errors, then return true; otherwise returns false.
     */
    bool buildScText(pANTLR3_BASE_TREE tree);

    //! Determine sentence type
    eSentenceType determineSentenceType(pANTLR3_BASE_TREE node);

    //! Process node as scs sentence level 1
    void processSentenceLevel1(pANTLR3_BASE_TREE node);
    //! Process node as scs sentence level 2-6
    void processSentenceLevel2_7(pANTLR3_BASE_TREE node);
    //! Process assignment sentence
    void processSentenceAssign(pANTLR3_BASE_TREE node);


    // --------- helper functions -----------
    //! Function that resolve sc-addr for element with specified identifier
    sc_addr resolveScAddr(const String &idtf);
    //! Create new sc-addr of element
    sc_addr createScAddr(sElement *el);
    //! Determines sc-type of element
    void determineElementType(sElement *el);

    //! Create new empty element
    sElement* _createElement(const String &idtf);

    /*! Append new node into elements
     * @param idtf Identifier of element
     * @returns Returns id of element
     */
    sElement* _addNode(const String &idtf);

    /*! Append new edge into elements
     * @param source Pointer to source element
     * @param target Pointer to target element
     * @param type Type of adge
     * @param idtf Identifier of element
     * @returns Returns id of element
     */
    sElement* _addEdge(sElement *source, sElement *target, sc_type type, const String &idtf);

    /*! Append new link into elements
     * @param is_file Flag to determine data type. If this flag is true, then data is a file path; otherwise
     * \p data contains link data
     * @param data Link data
     * @returns Returns id of created element
     */
    sElement* _addLink(bool is_file, const String &data);


    //! Parse subtree of antrl tree, and returns pointer to created sc-element, that designate this subtree
    sElement* parseElementTree(pANTLR3_BASE_TREE tree);

    //! Returns sc-type of arc, by preffix
    sc_type _getArcPreffixType(const String &preffix) const;
    //! Returns sc-type of element by type set identifier
    sc_type _getTypeBySetIdtf(const String &setIdtf) const;
    //! Returns arc type by connector token
    sc_type _getTypeByConnector(const String &connector);

    //! Dump to dot
    void dumpDot(pANTLR3_BASE_TREE tree);
    void dumpNode(pANTLR3_BASE_TREE node, std::ofstream &stream);
    void dumpScs(const String &fileName);

public:
    typedef std::set<sc_addr, sc_add_compare> tScAddrSet;
    typedef std::map<String, sc_addr> tStringAddrMap;
    typedef std::map<String, sElement*> tElementIdtfMap;
    typedef std::set<sElement*> tElementSet;
    typedef std::map<String, String> tAssignMap;

private:
    //! Set of created elements
    tScAddrSet mScAddrs;
    //! Map of elements description
    tElementIdtfMap mElementIdtf;
    //! Set of elements
    tElementSet mElementSet;
    //! Global counter of elements id's
    static uint32 msIdCounter;
    //! Map that contains global identifiers
    static tStringAddrMap msGlobalIdtfAddrs;
    //! Map that contains system identifiers
    tStringAddrMap mSysIdtfAddrs;
    //! Map that contains local identifiers
    tStringAddrMap mLocalIdtfAddrs;
    //! Map to store assignments
    tAssignMap mAssignments;

};

// -----------------

class SCsTranslatorFactory : public iTranslatorFactory
{
public:
    explicit SCsTranslatorFactory();
    virtual ~SCsTranslatorFactory();

    //! @copydoc iTranslatorFactory::createInstance
    iTranslator* createInstance();
    //! @copydoc iTranslatorFactory::getFileExt
    const String& getFileExt() const;
};

#endif // _scs_translator_h_
