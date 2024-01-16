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

sc_event * ui_translator_sc2scs_event = (sc_event *)null_ptr;
sc_event * ui_translator_sc2scg_json_event = (sc_event *)null_ptr;
sc_event * ui_translator_sc2scn_json_event = (sc_event *)null_ptr;

void ui_initialize_translators()
{
  ui_translator_sc2scs_event = sc_event_new(
      s_default_ctx, keynode_command_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, uiSc2ScsTranslator::ui_translate_sc2scs, 0);
  ui_translator_sc2scg_json_event = sc_event_new(
      s_default_ctx,
      keynode_command_initiated,
      SC_EVENT_ADD_OUTPUT_ARC,
      0,
      uiSc2SCgJsonTranslator::ui_translate_sc2scg_json,
      0);
  ui_translator_sc2scn_json_event = sc_event_new(
      s_default_ctx,
      keynode_command_initiated,
      SC_EVENT_ADD_OUTPUT_ARC,
      0,
      uiSc2SCnJsonTranslator::ui_translate_sc2scn,
      0);
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

sc_result ui_translate_command_resolve_arguments(
    sc_addr cmd_addr,
    sc_addr * output_fmt_addr,
    sc_addr * source_addr,
    sc_addr * lang_addr)
{
  sc_bool fmt_found = SC_FALSE;
  sc_bool source_found = SC_FALSE;
  SC_ADDR_MAKE_EMPTY(*lang_addr);

  // resolve output format
  sc_iterator5 * it = sc_iterator5_f_a_a_a_f_new(
      s_default_ctx,
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
  it = sc_iterator5_f_a_a_a_f_new(
      s_default_ctx,
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

  // resolve language
  it = sc_iterator5_f_a_a_a_f_new(
      s_default_ctx,
      cmd_addr,
      sc_type_arc_pos_const_perm,
      sc_type_node | sc_type_const,
      sc_type_arc_pos_const_perm,
      keynode_rrel_user_lang);

  while (sc_iterator5_next(it) == SC_TRUE)
  {
    *lang_addr = sc_iterator5_value(it, 2);
  }

  sc_iterator5_free(it);

  return SC_RESULT_OK;
}

sc_result ui_get_main_identifier_link(sc_memory_context * ctx, sc_addr el, sc_addr lang_addr, sc_addr * main_idtf_addr)
{
  sc_result result = SC_RESULT_ERROR;

  sc_iterator5 * it5 = sc_iterator5_f_a_a_a_f_new(
      ctx, el, sc_type_arc_common | sc_type_const, sc_type_link, sc_type_arc_pos_const_perm, keynode_nrel_main_idtf);
  while (sc_iterator5_next(it5) == SC_TRUE)
  {
    sc_addr link_addr = sc_iterator5_value(it5, 2);
    sc_iterator3 * it3 = sc_iterator3_f_a_f_new(ctx, lang_addr, sc_type_arc_pos_const_perm, link_addr);

    if (sc_iterator3_next(it3) == SC_TRUE)
    {
      *main_idtf_addr = link_addr;
      result = SC_RESULT_OK;
      sc_iterator3_free(it3);
      break;
    }
    sc_iterator3_free(it3);
  }

  sc_iterator5_free(it5);

  return result;
}

sc_bool ui_translate_get_identifier(sc_addr el, sc_addr lang_addr, String & idtf)
{
  sc_addr idtf_addr;
  sc_stream * idtf_stream = nullptr;
  sc_uint32 idtf_length = 0;
  sc_uint32 read_bytes = 0;
  sc_bool result = SC_FALSE;
  sc_char buffer[32];

  idtf = "";
  if (ui_get_main_identifier_link(s_default_ctx, el, lang_addr, &idtf_addr) == SC_RESULT_OK
      || sc_helper_get_system_identifier_link(s_default_ctx, el, &idtf_addr) == SC_RESULT_OK)
  {
    if (sc_memory_get_link_content(s_default_ctx, idtf_addr, &idtf_stream) == SC_RESULT_OK)
    {
      sc_stream_get_length(idtf_stream, &idtf_length);
      while (sc_stream_eof(idtf_stream) == SC_FALSE)
      {
        sc_stream_read_data(idtf_stream, buffer, 32, &read_bytes);
        idtf.append(buffer, read_bytes);
      }
      sc_stream_free(idtf_stream);

      result = SC_TRUE;
    }
  }

  if (result == SC_FALSE)
  {
    idtf = "";
    return SC_FALSE;
  }

  return result;
}
