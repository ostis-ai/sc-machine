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

sc_uint16 sc_element_get_iterator_refs(sc_element *element)
{
    return element->flags.refs.it;
}

sc_bool sc_element_is_valid(sc_element *element)
{
    return (element->flags.type == 0 || element->flags.type & sc_flag_request_deletion) ? SC_FALSE : SC_TRUE;
}

sc_bool sc_element_itref_add(sc_element *element)
{
    if (element->flags.refs.it == G_MAXUINT16)
        return SC_FALSE;

    element->flags.refs.it++;
    return SC_TRUE;
}

sc_bool sc_element_itref_dec(sc_element *element)
{
    g_assert(element->flags.refs.it > 0);
    element->flags.refs.it--;

    return (element->flags.refs.it == 0) ? SC_TRUE : SC_FALSE;
}
