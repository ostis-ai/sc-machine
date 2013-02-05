/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#ifndef _uiSc2SCnJsonTranslator_h_
#define _uiSc2SCnJsonTranslator_h_

#include "uiTranslatorFromSc.h"

struct sScElementInfo
{
    sc_type type;
    sc_addr addr;
    sc_addr beg_addr;
    sc_addr end_addr;
    tScAddrList output_arcs;
    tScAddrList input_arcs;
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


    static sc_result ui_translate_sc2scn(sc_event *event, sc_addr arg);

protected:
    //! @copydoc uiTranslateFromSc::runImpl
    void runImpl();

    /*! Translate one sc-element semantic neighborhood
     * @param addr sc-addr of sc-element to translate
     * @param isKeyword Keyword flag
     * @return Returns string that json representaion of sc-element
     */
    String translateElement(sc_addr addr, bool isKeyword);

    /*! Translate sc-arc information, with attributes and begin/end element
     * @param arcInfo Pointer to arc information
     * @param isBackward Backward flag value
     * @return Returns string that json representaion of sc-arc
     */
    String translateArc(sScElementInfo *arcInfo, bool isBackward);

    //! Check if specified sc-element is translated
    bool isTranslated(sc_addr element) const;

    //! Collect information of trsnslated sc-elements and store it
    void collectScElementsInfo();

protected:
    //! List of stranslated sc-elements
    tScAddrList mTranslatedAddrsList;
    //! List of keywords
    tScAddrList mKeywordsList;
    //! Collection of objects information
    typedef std::map<sc_addr, sScElementInfo*> tScElemetsInfoMap;
    tScElemetsInfoMap mScElementsInfo;
    //! Pull of sc-elements information (used to prevent many memory allocations)
    sScElementInfo *mScElementsInfoPool;
};



#endif // _uiSc2SCnJsonTranslator_h_
