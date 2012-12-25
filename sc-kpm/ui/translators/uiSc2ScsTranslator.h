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

#ifndef _ui_translator_sc2scs_h_
#define _ui_translator_sc2scs_h_

#include "uiTypes.h"

/*!
 * \brief Class that translates sc-construction into
 * SCs-code.
 */
class Sc2ScsTranslator
{
public:
    explicit Sc2ScsTranslator();
    virtual ~Sc2ScsTranslator();

    /*! Run translation.
     * @param input_addr sc-addr of sc-construction, that need to be translated
     * @param format_addr sc-addr of output format
     */
    void translate(const sc_addr &input_addr, const sc_addr &format_addr);

    static sc_result ui_translate_sc2scs(sc_event *event, sc_addr arg);

protected:
    //! Collect objects that need to be translated
    void collectObjects();

    //! Process input data and generate output
    void run();

    //! Check if sc-element need to be translated
    bool isNeedToTranslate(const sc_addr &addr) const;

protected:
    //! Sc-addr of input construction
    sc_addr mInputConstructionAddr;
    //! Sc-addr of output format
    sc_addr mOutputFormatAddr;

    // Maps of elements to translate
    tScAddrToScTypeMap mArcs;
    tScAddrToScTypeMap mNodes;
    tScAddrToScTypeMap mLinks;

    //! Output scs
    String mOutputData;

    typedef std::map<sc_type, String> tScTypeToSCsConnectorMap;
    tScTypeToSCsConnectorMap mTypeToConnector;

};

#endif
