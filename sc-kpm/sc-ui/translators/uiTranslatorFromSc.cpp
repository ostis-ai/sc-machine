/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "uiTranslatorFromSc.h"
#include "uiKeynodes.h"

constexpr size_t MAX_TRIPLES_COUNT = 1000;

uiTranslateFromSc::uiTranslateFromSc()
{
}

uiTranslateFromSc::~uiTranslateFromSc()
{
}

void uiTranslateFromSc::translate(sc_addr const & input_addr, sc_addr const & format_addr, sc_addr const & lang_addr)
{
  mInputConstructionAddr = input_addr;
  mOutputFormatAddr = format_addr;
  mOutputLanguageAddr = lang_addr;

  collectObjects();

  runImpl();

  // write into sc-link
  sc_stream * result_data_stream =
      sc_stream_memory_new(mOutputData.c_str(), (sc_uint)mOutputData.size(), SC_STREAM_FLAG_READ, SC_FALSE);

  sc_addr result_addr = sc_memory_link_new(s_default_ctx);
  sc_memory_set_link_content(s_default_ctx, result_addr, result_data_stream);

  sc_stream_free(result_data_stream);

  // generate format info
  sc_addr arc_addr = sc_memory_arc_new(s_default_ctx, sc_type_arc_common | sc_type_const, result_addr, format_addr);
  sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, keynode_nrel_format, arc_addr);

  // generate translation
  arc_addr = sc_memory_arc_new(s_default_ctx, sc_type_arc_common | sc_type_const, mInputConstructionAddr, result_addr);
  sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, keynode_nrel_translation, arc_addr);
}

void uiTranslateFromSc::collectObjects()
{
  sc_iterator3 * it = sc_iterator3_f_a_a_new(s_default_ctx, mInputConstructionAddr, sc_type_arc_pos_const_perm, 0);
  sc_uint32 i = 0;
  while (sc_iterator3_next(it) == SC_TRUE && i != MAX_TRIPLES_COUNT)
  {
    sc_type el_type = 0;
    sc_addr addr = sc_iterator3_value(it, 2);

    //! TODO add error logging
    if (sc_memory_get_element_type(s_default_ctx, addr, &el_type) != SC_RESULT_OK)
      continue;

    if (!(el_type & sc_type_arc_mask))
      continue;

    ++i;

    mEdges[addr] = el_type;
  }
  sc_iterator3_free(it);
}

bool uiTranslateFromSc::isNeedToTranslate(sc_addr const & addr) const
{
  return mEdges.find(addr) != mEdges.end();
}

String uiTranslateFromSc::buildId(sc_addr const & addr)
{
  auto v = SC_ADDR_LOCAL_TO_INT(addr);
  StringStream ss;
  ss << v;
  return ss.str();
}
