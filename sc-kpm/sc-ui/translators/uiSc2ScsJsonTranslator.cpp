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
uiSc2ScsTranslator::uiSc2ScsTranslator() {}

uiSc2ScsTranslator::~uiSc2ScsTranslator() {}

void uiSc2ScsTranslator::runImpl()
{
  ScJson result;

  ScJson keywords = ScJson::array();
  // get command arguments (keywords)
  sc_iterator5 * it5 = sc_iterator5_a_a_f_a_f_new(
      s_default_ctx,
      sc_type_node | sc_type_const,
      sc_type_common_arc | sc_type_const,
      mInputConstructionAddr,
      sc_type_const_perm_pos_arc,
      keynode_action_nrel_result);
  if (sc_iterator5_next(it5) == SC_TRUE)
  {
    sc_iterator3 * it3 =
        sc_iterator3_f_a_a_new(s_default_ctx, sc_iterator5_value(it5, 0), sc_type_const_perm_pos_arc, 0);
    while (sc_iterator3_next(it3) == SC_TRUE)
    {
      sc_addr addr = sc_iterator3_value(it3, 2);

      if (sc_helper_check_arc(s_default_ctx, mInputConstructionAddr, addr, sc_type_const_perm_pos_arc) != SC_TRUE)
        continue;

      sc_type type;
      sc_memory_get_element_type(s_default_ctx, addr, &type);

      ScJson keyword;
      keyword["addr"] = uiSc2ScsTranslator::buildId(addr);
      keyword["type"] = type;

      keywords.push_back(keyword);
    }
    sc_iterator3_free(it3);
  }
  sc_iterator5_free(it5);

  result["keywords"] = keywords;

  ScJson triples = ScJson::array();
  tScAddrSet constrAddrs;
  // iterate all arcs and translate them
  auto const itEnd = mEdges.cend();
  for (auto it = mEdges.cbegin(); it != itEnd; ++it)
  {
    sc_addr const & arc_addr = it->first;
    sc_type arc_type = it->second;

    // skip non arc objects
    if (!(arc_type & sc_type_arc_mask))
      continue;

    sc_addr arc_beg, arc_end;
    // get begin and end arc elements
    if (sc_memory_get_arc_begin(s_default_ctx, arc_addr, &arc_beg) != SC_RESULT_OK)
      continue;  //! TODO logging

    if (sc_memory_get_arc_end(s_default_ctx, arc_addr, &arc_end) != SC_RESULT_OK)
      continue;  //! TODO logging

    sc_type beg_type, end_type;
    sc_memory_get_element_type(s_default_ctx, arc_beg, &beg_type);
    sc_memory_get_element_type(s_default_ctx, arc_end, &end_type);

    constrAddrs.insert(arc_beg);
    constrAddrs.insert(arc_end);

    ScJson triple = ScJson::array();
    triple.push_back({{"addr", buildId(arc_beg)}, {"type", beg_type}});
    triple.push_back({{"addr", buildId(arc_addr)}, {"type", arc_type}});
    triple.push_back({{"addr", buildId(arc_end)}, {"type", end_type}});

    triples.push_back(triple);
  }

  result["triples"] = triples;

  if (SC_ADDR_IS_EMPTY(mOutputLanguageAddr))
  {
    result["identifiers"] = ScJson::array();
  }
  else
  {
    ScJson identifiers;
    auto constrItEnd = constrAddrs.cend();
    for (auto it = constrAddrs.cbegin(); it != constrItEnd; ++it)
    {
      sc_addr const addr = *it;
      String idtf;
      bool idtf_exists = getIdentifier(addr, mOutputLanguageAddr, idtf);

      if (idtf_exists)
      {
        identifiers[buildId(*it)] = idtf;
      }
      else
      {
        identifiers[buildId(*it)] = ScJson();
      }
    }

    result["identifiers"] = identifiers;
  }

  mOutputData = result.dump();
}

bool uiSc2ScsTranslator::getIdentifier(sc_addr const & addr, sc_addr const & lang_addr, String & idtf)
{
  auto it = mIdentifiers.find(addr);
  if (it != mIdentifiers.end())
  {
    idtf = it->second;
    return true;
  }

  bool result = ui_translate_get_identifier(addr, lang_addr, idtf);

  if (result)
  {
    mIdentifiers[addr] = idtf;
  }

  return result;
}

// -------------------------------------------------------
sc_result uiSc2ScsTranslator::ui_translate_sc2scs(sc_event_subscription const *, sc_addr arg)
{
  sc_addr cmd_addr, input_addr, format_addr, lang_addr;

  if (sc_memory_get_arc_end(s_default_ctx, arg, &cmd_addr) != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  if (ui_check_cmd_type(cmd_addr, keynode_command_translate_from_sc) != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  if (ui_translate_command_resolve_arguments(cmd_addr, &format_addr, &input_addr, &lang_addr) != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  if (format_addr == keynode_format_scs_json)
  {
    uiSc2ScsTranslator translator;
    translator.translate(input_addr, format_addr, lang_addr);
  }

  return SC_RESULT_OK;
}
