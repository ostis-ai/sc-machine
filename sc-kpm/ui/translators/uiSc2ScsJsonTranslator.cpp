/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "uiPrecompiled.h"
#include "uiSc2ScsJsonTranslator.h"

#include "uiTranslators.h"
#include "uiKeynodes.h"
#include "uiUtils.h"

// --------------------
uiSc2ScsTranslator::uiSc2ScsTranslator()
{
}

uiSc2ScsTranslator::~uiSc2ScsTranslator()
{

}

void uiSc2ScsTranslator::runImpl()
{
    StringStream ss;
    ss << "{";
    ss << "\"keywords\" : [";

    bool first = true;
    // get command arguments (keywords)
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
        {
            sc_addr addr = sc_iterator3_value(it3, 2);

            if (sc_helper_check_arc(s_default_ctx, mInputConstructionAddr, addr, sc_type_arc_pos_const_perm) != SC_TRUE)
                continue;

            sc_type type;
            sc_memory_get_element_type(s_default_ctx, addr, &type);

            if (!first)
                ss << ",";
            else
                first = false;

            ss << "{ \"addr\": \"" << uiSc2ScsTranslator::buildId(addr) << "\", \"type\" : " << type << "}";
        }
        sc_iterator3_free(it3);
    }
    sc_iterator5_free(it5);

    ss << "], \"triples\": [";

    first = true;
    // iterate all arcs and translate them
    tScAddrToScTypeMap::iterator it, itEnd = mObjects.end();
    for (it = mObjects.begin(); it != itEnd; ++it)
    {
        const sc_addr &arc_addr = it->first;
        sc_type arc_type = it->second;

        // skip non arc objects
        if (!(arc_type & sc_type_arc_mask))
            continue;

        sc_addr arc_beg, arc_end;
        // get begin and end arc elements
        if (sc_memory_get_arc_begin(s_default_ctx, arc_addr, &arc_beg) != SC_RESULT_OK)
            continue; //! TODO logging

        if (isNeedToTranslate(arc_beg) == false)
            continue; //! TODO logging

        if (sc_memory_get_arc_end(s_default_ctx, arc_addr, &arc_end) != SC_RESULT_OK)
            continue; //! TODO logging

        if (isNeedToTranslate(arc_end) == false)
            continue; //! TODO logging

        sc_type beg_type, end_type;
        tScAddrToScTypeMap::iterator itTmp = mObjects.find(arc_beg);
        if (itTmp != mObjects.end())
            beg_type = itTmp->second;
        else
            sc_memory_get_element_type(s_default_ctx, arc_beg, &beg_type);
        itTmp = mObjects.find(arc_end);
        if (itTmp != mObjects.end())
            end_type = itTmp->second;
        else
            sc_memory_get_element_type(s_default_ctx, arc_end, &end_type);

        if (!first)
            ss << ", ";
        else
            first = false;

        ss << "[{ \"addr\": \"" << buildId(arc_beg) << "\", \"type\": " << beg_type << "}, ";
        ss << "{ \"addr\": \"" << buildId(arc_addr) << "\", \"type\": " << arc_type << "}, ";
        ss << "{ \"addr\": \"" << buildId(arc_end) << "\", \"type\": " << end_type << "}]";

    }

    ss << "]}";
    mOutputData = ss.str();
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
sc_result uiSc2ScsTranslator::ui_translate_sc2scs(const sc_event *event, sc_addr arg)
{
    sc_addr cmd_addr, input_addr, format_addr;

    if (sc_memory_get_arc_end(s_default_ctx, arg, &cmd_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (ui_check_cmd_type(cmd_addr, keynode_command_translate_from_sc) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (ui_translate_command_resolve_arguments(cmd_addr, &format_addr, &input_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (format_addr == keynode_format_scs_json)
    {
        uiSc2ScsTranslator translator;
        translator.translate(input_addr, format_addr);
    }

    return SC_RESULT_OK;
}
