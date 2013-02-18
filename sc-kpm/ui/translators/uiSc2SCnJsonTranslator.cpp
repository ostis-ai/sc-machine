/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

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
    : mScElementsInfoPool(0)
{
    SC_ADDR_MAKE_EMPTY(mKeywordAddr);
}

uiSc2SCnJsonTranslator::~uiSc2SCnJsonTranslator()
{
    if (mScElementsInfoPool)
        delete []mScElementsInfoPool;
}

void uiSc2SCnJsonTranslator::runImpl()
{
    // get command arguments
    sc_iterator5 *it5 = sc_iterator5_a_a_f_a_f_new(sc_type_node | sc_type_const,
                                                   sc_type_arc_common | sc_type_const,
                                                   mInputConstructionAddr,
                                                   sc_type_arc_pos_const_perm,
                                                   keynode_question_nrel_answer);
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

    collectScElementsInfo();

    mOutputData = "[";

    tScAddrList::iterator it, itEnd = mKeywordsList.end();
    for (it = mKeywordsList.begin(); it != itEnd; ++it)
    {
        if (it != mKeywordsList.begin())
            mOutputData += ",";
        mKeywordAddr = *it;
        mOutputData += translateElement(*it, true);
    }

    // @todo check if there are some nodes, that not translated

    mOutputData += "]";
}

String uiSc2SCnJsonTranslator::translateElement(sc_addr addr, bool isKeyword)
{
    tStringStringMap attrs;
    String result = "";

    mTranslatedAddrsList.push_back(addr);

    result += "{";
    result += "\"id\": \"" + buildId(addr) + "\",";
    result += "\"keyword\": ";
    result += isKeyword ? "true" : "false";
    result += ", \"SCArcs\" : [";

    sScElementInfo *elInfo = mScElementsInfo[addr];
    sScElementInfo *arcInfo = 0;
    // iterate input arcs
    bool first = true;
    tScAddrList::const_iterator it, itEnd = elInfo->input_arcs.end();
    for (it = elInfo->input_arcs.begin(); it != itEnd; ++it)
    {
        arcInfo = mScElementsInfo[*it];
        if (/*isTranslated(arcInfo->beg_addr) || */isTranslated(arcInfo->addr) || SC_ADDR_IS_EQUAL(arcInfo->beg_addr, mKeywordAddr))
            continue;

        if (!first)
            result += ",";
        result += translateArc(arcInfo, true);
        first = false;
    }
    itEnd = elInfo->output_arcs.end();
    for (it = elInfo->output_arcs.begin(); it != itEnd; ++it)
    {
        arcInfo = mScElementsInfo[*it];
        if (/*isTranslated(arcInfo->end_addr) || */isTranslated(arcInfo->addr) || SC_ADDR_IS_EQUAL(arcInfo->end_addr, mKeywordAddr))
            continue;

        if (!first)
            result += ",";
        result += translateArc(arcInfo, false);
        first = false;
    }

    result += "],";
    StringStream ss;
    ss << elInfo->type;
    result += "\"type\":" + ss.str();
    result += "}";

    return result;
}

String uiSc2SCnJsonTranslator::translateArc(sScElementInfo *arcInfo, bool isBackward)
{
    sc_addr elAddr = isBackward ? arcInfo->beg_addr : arcInfo->end_addr;
    bool hasAttributes = false;
    String result = "{";
    String attrs = "";

    mTranslatedAddrsList.push_back(arcInfo->addr);


    // collect all input constant, positive arcs
    tScAddrList::iterator it, itEnd = arcInfo->input_arcs.end();
    for (it = arcInfo->input_arcs.begin(); it != itEnd; ++it)
    {
        sScElementInfo *inputArcInfo = mScElementsInfo[*it];
        sScElementInfo *begInfo = mScElementsInfo[inputArcInfo->beg_addr];
        assert(inputArcInfo);
        if (inputArcInfo == nullptr)
        {
            //printf("Error while get input arc information, when resolve attribute in SC2SCnJson translator");
            continue; // we need stable server
        }

        if ((inputArcInfo->type & sc_type_arc_pos_const_perm) &&
            ((begInfo->type & sc_type_node_struct_mask) & (sc_type_node_norole | sc_type_node_role)) &&
            (SC_ADDR_IS_NOT_EQUAL(mKeywordAddr, begInfo->addr)))
        {
            if (hasAttributes)
                attrs += ",";
            attrs += "{ \"id\": \"" + buildId(inputArcInfo->beg_addr) + "\"}";

            hasAttributes = true;
        }
    }

    if (hasAttributes)
        result += "\"SCAttributes\" : [" + attrs + "],";

    StringStream ss;
    ss << arcInfo->type;

    result += "\"id\": \"" + buildId(arcInfo->addr) + "\",";
    result += "\"type\":" + ss.str();
    result += ",\"backward\":";
    result += isBackward ? "true" : "false";
    result += ",\"SCNode\":" + translateElement(elAddr, false);
    result += "}";

    return result;
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

void uiSc2SCnJsonTranslator::collectScElementsInfo()
{
    sc_uint32 elementsCount = mObjects.size();
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
        elType = it->second;
        // skip nodes and links
        if (!(elType & sc_type_arc_mask))
            continue;
        arcAddr = it->first;

        // get begin/end addrs
        if (sc_memory_get_arc_begin(arcAddr, &begAddr) != SC_RESULT_OK)
            continue; // @todo process errors
        if (sc_memory_get_arc_end(arcAddr, &endAddr) != SC_RESULT_OK)
            continue; // @todo process errors

        sScElementInfo *elInfo = mScElementsInfo[arcAddr];
        elInfo->beg_addr = begAddr;
        elInfo->end_addr = endAddr;

        sScElementInfo *begInfo = mScElementsInfo[begAddr];
        sScElementInfo *endInfo = mScElementsInfo[endAddr];

        // check if arc is not broken
        if (begInfo == 0 || endInfo == 0)
            continue;
        endInfo->input_arcs.push_back(arcAddr);
        begInfo->output_arcs.push_back(arcAddr);
    }
}

// -------------------------------------
sc_result uiSc2SCnJsonTranslator::ui_translate_sc2scn(sc_event *event, sc_addr arg)
{
    sc_addr cmd_addr, input_addr, format_addr;

    if (sc_memory_get_arc_end(arg, &cmd_addr) != SC_RESULT_OK)
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


