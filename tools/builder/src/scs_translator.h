/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

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

struct sBuffer
{
	std::vector<char> data;

	sBuffer()
	{		
	}

	sBuffer(char const * in_data, sc_uint in_len)
	{
		set(in_data, in_len);
	}

	void set(char const * in_data, sc_uint in_len)
	{
		data.assign(in_data, in_data + in_len);
	}


};

struct sElement
{
    sc_type type;
    sc_addr addr;
    String idtf;

    // arc info
    sElement *arc_src;
    sElement *arc_trg;

    // link info
    bool link_is_file;
	String file_path;
    sBuffer link_data;

    bool ignore;

    sElement()
        : type(0)
        , arc_src(0)
        , arc_trg(0)
        , link_is_file(false)
        , ignore(false)
    {
        SC_ADDR_MAKE_EMPTY(addr);
    }

    sElement* clone()
    {
        sElement *cl = new sElement();

        cl->type = type;
        cl->addr = addr;
        cl->idtf = idtf;
        cl->arc_src = arc_src;
        cl->arc_trg = arc_trg;
        cl->link_is_file = link_is_file;
        cl->link_data = link_data;
        cl->ignore = ignore;

        return cl;
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
        SentenceAssign = 3,
        SentenceEOF = 4
    } eSentenceType;

public:
    explicit SCsTranslator(sc_memory_context *ctx);
    virtual ~SCsTranslator();

    //! @copydoc iTranslator::translateImpl
    bool translateImpl();
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

    //! Process attrs idtf list
    void processAttrsIdtfList(bool ignore_first, pANTLR3_BASE_TREE node, sElement *el_obj, const String &connector, bool generate_order);

    //! Process node as scs sentence level 1
    void processSentenceLevel1(pANTLR3_BASE_TREE node);
    //! Process node as scs sentence level 2-6
    void processSentenceLevel2_7(pANTLR3_BASE_TREE node);
    //! Process assignment sentence
    void processSentenceAssign(pANTLR3_BASE_TREE node);


    // --------- helper functions -----------
    //! Function that resolve sc-addr for element with specified identifier
    sc_addr resolveScAddr(sElement *el);
    //! Create new sc-addr of element
    sc_addr createScAddr(sElement *el);
    //! Determines sc-type of element
    void determineElementType(sElement *el);

    //! Create new empty element
    sElement* _createElement(const String &idtf, sc_type type);

    /*! Append new node into elements
     * @param idtf Identifier of element
     * @param type sc-type of node
     * @returns Returns id of element
     */
    sElement* _addNode(const String &idtf, sc_type type = 0);

    /*! Append new edge into elements
     * @param source Pointer to source element
     * @param target Pointer to target element
     * @param type Type of edge
     * @param is_reversed Flag to reverse edge direction
     * @param idtf Identifier of element
     * @returns Returns id of element
     */
    sElement* _addEdge(sElement *source, sElement *target, sc_type type, bool is_reversed, const String &idtf);

    /*! Append new link into elements
     * @param idtf Link identifier if it exists
     * @param data Link data
     * @returns Returns id of created element
     */
    sElement* _addLink(const String & idtf, const sBuffer & data);
	sElement* _addLinkFile(const String & idtf, const String & filePath);
	sElement* _addLinkString(const String & idtf, const String & str);


    //! Parse subtree of antrl tree, and returns pointer to created sc-element, that designate this subtree
    sElement* parseElementTree(pANTLR3_BASE_TREE tree, const String *assignIdtf = 0);

    //! Returns sc-type of arc, by preffix
    sc_type _getArcPreffixType(const String &preffix) const;
    //! Returns sc-type of element by type set identifier
    sc_type _getTypeBySetIdtf(const String &setIdtf) const;
    //! Returns arc type by connector token
    sc_type _getTypeByConnector(const String &connector);
    //! If connector reversed, then returns true; otherwise returns false
    bool _isConnectorReversed(const String &connector);

    //! Get absolute file path from it url
    bool _getAbsFilePath(const String &url, String &abs_path);

    //! Check if identifier is variable
    bool _isIdentifierVar(const String &idtf) const;

	/*! Parse binary data from string content
	 * @param data String that contains data for parsing
	 * @param outBuffer Output buffer to store parsed data as binary
	 * @returns If data parsed then return true, otherwise - false
	 */
	bool parseContentBinaryData(String const & data, sBuffer & outBuffer);

    //! Dump to dot
    void dumpDot(pANTLR3_BASE_TREE tree);
    void dumpNode(pANTLR3_BASE_TREE node, std::ofstream &stream);
    void dumpScs(const String &fileName);

public:
    typedef std::set<sc_addr, sc_add_compare> tScAddrSet;
    typedef std::map<String, sElement*> tElementIdtfMap;
    typedef std::set<sElement*> tElementSet;
    typedef std::map<String, String> tAssignMap;
    typedef std::map<String, sc_type> tScTypesMap;

private:
    //! Set of created elements
    tScAddrSet mScAddrs;
    //! Map of elements description
    tElementIdtfMap mElementIdtf;
    //! Set of elements
    tElementSet mElementSet;

    //! Map to store assignments
    tAssignMap mAssignments;
    //! Counter of ... identifiers
    static long long msAutoIdtfCount;
};

// -----------------

class SCsTranslatorFactory : public iTranslatorFactory
{
public:
    explicit SCsTranslatorFactory();
    virtual ~SCsTranslatorFactory();

    static const String EXTENSION;

    //! @copydoc iTranslatorFactory::createInstance
    iTranslator* createInstance(sc_memory_context *ctx);
    //! @copydoc iTranslatorFactory::getFileExt
    const String& getFileExt() const;
};

#endif // _scs_translator_h_
