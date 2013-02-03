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

#include "uiPrecompiled.h"
#include "uiSc2ScsJsonTranslator.h"

#include "uiTranslators.h"
#include "uiKeynodes.h"

// --------------------
uiSc2ScsTranslator::uiSc2ScsTranslator()
{
    mTypeToConnector[sc_type_arc_common] = ">";
    mTypeToConnector[sc_type_arc_pos_const_perm] = "->";
    mTypeToConnector[sc_type_edge_common] = "<>";
    mTypeToConnector[sc_type_arc_access] = "..>";

    mTypeToConnector[sc_type_edge_common | sc_type_const] = "<=>";
    mTypeToConnector[sc_type_edge_common | sc_type_var] = "_<=>";
    mTypeToConnector[sc_type_arc_common | sc_type_const] = "=>";
    mTypeToConnector[sc_type_arc_common | sc_type_var] = "_=>";
    mTypeToConnector[sc_type_arc_access | sc_type_const | sc_type_arc_neg | sc_type_arc_perm] = "-|>";
    mTypeToConnector[sc_type_arc_access | sc_type_var | sc_type_arc_neg | sc_type_arc_perm] = "_-|>";
    mTypeToConnector[sc_type_arc_access | sc_type_const | sc_type_arc_fuz | sc_type_arc_perm] = "-/>";
    mTypeToConnector[sc_type_arc_access | sc_type_var | sc_type_arc_fuz | sc_type_arc_perm] = "_-/>";
    mTypeToConnector[sc_type_arc_access | sc_type_const | sc_type_arc_pos | sc_type_arc_temp] = "~>";
    mTypeToConnector[sc_type_arc_access | sc_type_var | sc_type_arc_pos | sc_type_arc_temp] = "_~>";
    mTypeToConnector[sc_type_arc_access | sc_type_const | sc_type_arc_neg | sc_type_arc_temp] = "~|>";
    mTypeToConnector[sc_type_arc_access | sc_type_var | sc_type_arc_neg | sc_type_arc_temp] = "_~|>";
    mTypeToConnector[sc_type_arc_access | sc_type_const | sc_type_arc_fuz | sc_type_arc_temp] = "~/>";
    mTypeToConnector[sc_type_arc_access | sc_type_var | sc_type_arc_fuz | sc_type_arc_temp] = "_~/>";
}

uiSc2ScsTranslator::~uiSc2ScsTranslator()
{

}

void uiSc2ScsTranslator::runImpl()
{
    //! TODO logging sc-element, that can't be translated

    // iterate all arcs and translate them
    tScAddrToScTypeMap::iterator it, itEnd = mArcs.end();

    mOutputData = "[";

    for (it = mArcs.begin(); it != itEnd; ++it)
    {
        const sc_addr &arc_addr = it->first;
        sc_type arc_type = it->second;

        sc_addr arc_beg, arc_end;
        // get begin and end arc elements
        if (sc_memory_get_arc_begin(arc_addr, &arc_beg) != SC_RESULT_OK)
            continue; //! TODO logging

        if (isNeedToTranslate(arc_beg) == false)
            continue; //! TODO logging

        if (sc_memory_get_arc_end(arc_addr, &arc_end) != SC_RESULT_OK)
            continue; //! TODO logging

        if (isNeedToTranslate(arc_end) == false)
            continue; //! TODO logging

        // determine arc type
        String arc_connector = "<>";
        tScTypeToSCsConnectorMap::const_iterator itCon = mTypeToConnector.find(arc_type);
        if (itCon != mTypeToConnector.end())
            arc_connector = itCon->second;

        if (it != mArcs.begin())
            mOutputData += ",";
        mOutputData += "[\"" + buildId(arc_beg) + "\", \"" + arc_connector + "\", \"" + buildId(arc_end) + "\"]";
    }

    mOutputData += "]";

}

void uiSc2ScsTranslator::resolveSystemIdentifier(const sc_addr &addr, String &idtf)
{
    tSystemIdentifiersMap::iterator it = mSystemIdentifiers.find(addr);
    if (it != mSystemIdentifiers.end())
    {
        idtf = it->second;
        return;
    }

    ui_translate_resolve_system_identifier(addr, idtf);
    mSystemIdentifiers[addr] = idtf;
}


// -------------------------------------------------------
sc_result uiSc2ScsTranslator::ui_translate_sc2scs(sc_event *event, sc_addr arg)
{
    sc_addr cmd_addr, input_addr, format_addr;

    if (sc_memory_get_arc_end(arg, &cmd_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (ui_translate_command_resolve_arguments(cmd_addr, &format_addr, &input_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (format_addr == ui_keynode_format_scs_json)
    {
        uiSc2ScsTranslator translator;
        translator.translate(input_addr, format_addr);
    }

    return SC_RESULT_OK;
}
