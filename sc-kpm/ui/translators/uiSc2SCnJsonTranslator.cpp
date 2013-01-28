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
#include "uiSc2SCnJsonTranslator.h"

#include "uiTranslators.h"
#include "uiKeynodes.h"

uiSc2SCnJsonTranslator::uiSc2SCnJsonTranslator()
{

}

uiSc2SCnJsonTranslator::~uiSc2SCnJsonTranslator()
{

}

void uiSc2SCnJsonTranslator::runImpl()
{
    // get command arguments
    sc_iterator5 *it5 = sc_iterator5_a_a_f_a_f_new(sc_type_node | sc_type_const,
                                                   sc_type_arc_common | sc_type_const,
                                                   mInputConstructionAddr,
                                                   sc_type_arc_pos_const_perm,
                                                   ui_keynode_question_nrel_answer);
    if (sc_iterator5_next(it5) == SC_TRUE)
    {
        sc_iterator3 *it3 = sc_iterator3_f_a_a_new(sc_iterator5_value(it5, 0),
                                                   sc_type_arc_pos_const_perm,
                                                   0);
        while (sc_iterator3_next(it3) == SC_TRUE)
            mKeywordsList.push_back(sc_iterator3_value(it3, 2));
        sc_iterator3_free(it3);
    }
    sc_iterator5_free(it5);

    mOutputData = "[";

    tScAddrList::iterator it, itEnd = mKeywordsList.end();
    for (it = mKeywordsList.begin(); it != itEnd; ++it)
        translateKeyword(*it);

    // @todo check if there are some nodes, that not translated

    mOutputData += "]";
}

void uiSc2SCnJsonTranslator::translateKeyword(sc_addr keyword_addr)
{
    tStringStringMap attrs;

    mTranslatedAddrsList.push_back(keyword_addr);

    attrs["id"] = buildId(keyword_addr);
    attrs["keywrod"] = "true";


    mOutputData += "{";
    tStringStringMap::iterator itAttrs, itAttrsEnd = attrs.end();
    for (itAttrs = attrs.begin(); itAttrs != itAttrsEnd; ++itAttrs)
    {
        if (itAttrs != attrs.begin())
            mOutputData += ", ";
        mOutputData += "\"" + itAttrs->first + "\": ";
        mOutputData += "\"" + itAttrs->second + "\"";
    }
    mOutputData += "}";

    if (keyword_addr != mKeywordsList.back())
        mOutputData += ", ";
}

bool uiSc2SCnJsonTranslator::isTranslated(sc_addr element) const
{
    tScAddrList::const_iterator it, itEnd = mTranslatedAddrsList.end();
    for (it = mTranslatedAddrsList.begin(); it != itEnd; ++it)
    {
        if (*it == element)
            return true;
    }

    return false;
}

// -------------------------------------
sc_result uiSc2SCnJsonTranslator::ui_translate_sc2scn(sc_event *event, sc_addr arg)
{
    sc_addr cmd_addr, input_addr, format_addr;

    if (sc_memory_get_arc_end(arg, &cmd_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (ui_translate_command_resolve_arguments(cmd_addr, &format_addr, &input_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (format_addr == ui_keynode_format_scn_json)
    {
        uiSc2SCnJsonTranslator translator;
        translator.translate(input_addr, format_addr);
    }

    return SC_RESULT_OK;
}


