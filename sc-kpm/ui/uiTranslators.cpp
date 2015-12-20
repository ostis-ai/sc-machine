/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "uiPrecompiled.h"
#include "uiTranslators.h"
#include "uiKeynodes.h"

#include "translators/uiSc2ScsJsonTranslator.h"
#include "translators/uiSc2SCgJsonTranslator.h"
#include "translators/uiSc2SCnJsonTranslator.h"

sc_event *ui_translator_sc2scs_event = (sc_event*)null_ptr;
sc_event *ui_translator_sc2scg_json_event = (sc_event*)null_ptr;
sc_event *ui_translator_sc2scn_json_event = (sc_event*)null_ptr;

void ui_initialize_translators()
{
    ui_translator_sc2scs_event = sc_event_new(s_default_ctx, keynode_command_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, uiSc2ScsTranslator::ui_translate_sc2scs, 0);
    ui_translator_sc2scg_json_event = sc_event_new(s_default_ctx, keynode_command_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, uiSc2SCgJsonTranslator::ui_translate_sc2scg_json, 0);
    ui_translator_sc2scn_json_event = sc_event_new(s_default_ctx, keynode_command_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, uiSc2SCnJsonTranslator::ui_translate_sc2scn, 0);
}

void ui_shutdown_translators()
{
    if (ui_translator_sc2scs_event)
        sc_event_destroy(ui_translator_sc2scs_event);
    if (ui_translator_sc2scg_json_event)
        sc_event_destroy(ui_translator_sc2scg_json_event);
    if (ui_translator_sc2scn_json_event)
        sc_event_destroy(ui_translator_sc2scn_json_event);
}

sc_result ui_translate_command_resolve_arguments(sc_addr cmd_addr, sc_addr *output_fmt_addr, sc_addr *source_addr)
{
    sc_iterator5 *it = (sc_iterator5*)null_ptr;
    sc_bool fmt_found = SC_FALSE;
    sc_bool source_found = SC_FALSE;

    // resolve output format
    it = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                    cmd_addr,
                                    sc_type_arc_pos_const_perm,
                                    sc_type_node | sc_type_const,
                                    sc_type_arc_pos_const_perm,
                                    keynode_rrel_output_format);

    while (sc_iterator5_next(it) == SC_TRUE)
    {
        *output_fmt_addr = sc_iterator5_value(it, 2);
        fmt_found = SC_TRUE;
    }

    sc_iterator5_free(it);

    if (fmt_found == SC_FALSE)
        return SC_RESULT_ERROR;

    // resolve input construction
    it = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                    cmd_addr,
                                    sc_type_arc_pos_const_perm,
                                    sc_type_node | sc_type_const,
                                    sc_type_arc_pos_const_perm,
                                    keynode_rrel_source_sc_construction);

    while (sc_iterator5_next(it) == SC_TRUE)
    {
        *source_addr = sc_iterator5_value(it, 2);
        source_found = SC_TRUE;
    }

    sc_iterator5_free(it);

    if (source_found == SC_FALSE)
        return SC_RESULT_ERROR;

    return SC_RESULT_OK;
}

sc_bool ui_translate_resolve_system_identifier(sc_addr el, String &sys_idtf)
{
    sc_addr sys_idtf_addr;
    sc_stream *idtf_stream = 0;
    sc_uint32 idtf_length = 0;
    sc_uint32 read_bytes = 0;
    sc_bool result = SC_FALSE;
    sc_char buffer[32];

    sys_idtf = "";
	if (sc_helper_get_system_identifier_link(s_default_ctx, el, &sys_idtf_addr) == SC_RESULT_OK)
    {
        if (sc_memory_get_link_content(s_default_ctx, sys_idtf_addr, &idtf_stream) == SC_RESULT_OK)
        {
            sc_stream_get_length(idtf_stream, &idtf_length);
            while (sc_stream_eof(idtf_stream) == SC_FALSE)
            {
                sc_stream_read_data(idtf_stream, buffer, 32, &read_bytes);
                sys_idtf.append(buffer, read_bytes);
            }
            sc_stream_free(idtf_stream);

            result = SC_TRUE;
        }
    }

    if (result == SC_FALSE)
    {
        StringStream ss;

        ss << el.seg << "_" << el.offset;
        sys_idtf = ss.str();
        return SC_FALSE;
    }

    return result;
}
