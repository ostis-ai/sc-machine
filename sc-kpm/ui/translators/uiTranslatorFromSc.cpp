/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

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
#include "uiTranslatorFromSc.h"
#include "uiKeynodes.h"

uiTranslateFromSc::uiTranslateFromSc()
{

}

uiTranslateFromSc::~uiTranslateFromSc()
{

}

void uiTranslateFromSc::translate(const sc_addr &input_addr, const sc_addr &format_addr)
{
    mInputConstructionAddr = input_addr;
    mOutputFormatAddr = format_addr;

    collectObjects();

    runImpl();

    // write into sc-link
    sc_stream *result_data_stream = 0;
    result_data_stream = sc_stream_memory_new(mOutputData.c_str(), mOutputData.size(), SC_STREAM_READ, SC_FALSE);

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
    sc_iterator3 *it = sc_iterator3_f_a_a_new(s_default_ctx, mInputConstructionAddr, sc_type_arc_pos_const_perm, 0);
    while (sc_iterator3_next(it) == SC_TRUE)
    {
        sc_type el_type = 0;
        sc_addr addr = sc_iterator3_value(it, 2);

        //! TODO add error logging
        if (sc_memory_get_element_type(s_default_ctx, addr, &el_type) != SC_RESULT_OK)
            continue;

        mObjects[addr] = el_type;

    }
    sc_iterator3_free(it);
}

bool uiTranslateFromSc::isNeedToTranslate(const sc_addr &addr) const
{
    return mObjects.find(addr) != mObjects.end();
}

String uiTranslateFromSc::buildId(const sc_addr &addr)
{
    uint32_t v = addr.seg | (addr.offset << 16);
    StringStream ss;
    ss << v;
    return ss.str();
}
