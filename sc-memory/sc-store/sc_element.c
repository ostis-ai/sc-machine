/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_element.h"
#include "sc_defines.h"
#include "sc_types.h"
#include "sc_segment.h"

#include <glib.h>

void sc_element_set_type(sc_element *element, sc_type type)
{
    g_assert(element != 0);
    element->flags.type = sc_flags_remove(type);
}

sc_bool sc_element_is_checksum_empty(sc_element *element)
{
    g_assert(element->flags.type & sc_type_link);
    sc_uint32 i = 0;
    for (; i < SC_CHECKSUM_LEN; ++i)
        if (element->content.data[i] != 0)
            return SC_FALSE;

    return SC_TRUE;
}

sc_bool sc_element_is_request_deletion(sc_element *element)
{
    return (element->flags.type & sc_flag_request_deletion) ? SC_TRUE : SC_FALSE;
}

sc_bool sc_element_is_valid(sc_element *element)
{
    return (element->flags.type == 0 || element->flags.type & sc_flag_request_deletion) ? SC_FALSE : SC_TRUE;
}

sc_uint16 sc_element_get_refs(sc_element_meta *element)
{
    return element->ref_count;
}

sc_bool sc_element_ref(sc_element_meta *element)
{
    if (element->ref_count == G_MAXUINT32)
        return SC_FALSE;

    element->ref_count++;
    return SC_TRUE;
}

sc_bool sc_element_unref(sc_element_meta *element)
{
    g_assert(element->ref_count > 0);
    element->ref_count--;

    return (element->ref_count == 0) ? SC_TRUE : SC_FALSE;
}
