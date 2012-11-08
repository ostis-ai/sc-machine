/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

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

#include "sc_iterator.h"
#include "sc_element.h"
#include "sc_storage.h"

#include <glib.h>

sc_iterator3* sc_iterator3_f_a_a_new(sc_addr el, sc_type arc_type, sc_type end_type)
{
    sc_iterator_param p1, p2, p3;

    p1.is_type = SC_FALSE;
    p1.addr = el;

    p2.is_type = SC_TRUE;
    p2.type = arc_type;

    p3.is_type = SC_TRUE;
    p3.type = end_type;

    return sc_iterator3_new(sc_iterator3_f_a_a, p1, p2, p3);
}

sc_iterator3* sc_iterator3_a_a_f_new(sc_type beg_type, sc_type arc_type, sc_addr el)
{
    sc_iterator_param p1, p2, p3;

    p1.is_type = SC_TRUE;
    p1.type = beg_type;

    p2.is_type = SC_TRUE;
    p2.type = arc_type;

    p3.is_type = SC_FALSE;
    p3.addr = el;

    return sc_iterator3_new(sc_iterator3_a_a_f, p1, p2, p3);
}

sc_iterator3* sc_iterator3_f_a_f_new(sc_addr el_beg, sc_type arc_type, sc_addr el_end)
{
    sc_iterator_param p1, p2, p3;

    p1.is_type = SC_FALSE;
    p1.addr = el_beg;

    p2.is_type = SC_TRUE;
    p2.type = arc_type;

    p3.is_type = SC_FALSE;
    p3.addr = el_end;

    return sc_iterator3_new(sc_iterator3_f_a_f, p1, p2, p3);
}

sc_iterator3* sc_iterator3_new(sc_iterator_type type, sc_iterator_param p1, sc_iterator_param p2, sc_iterator_param p3)
{
    // check types
    if (type > sc_iterator3_f_a_f) return (sc_iterator3*)0;

    // check params with template
    switch (type)
    {
    case sc_iterator3_f_a_a:
        if (p1.is_type || !p2.is_type || !p3.is_type) return (sc_iterator3*)0;
        break;
    case sc_iterator3_a_a_f:
        if (!p1.is_type || !p2.is_type || p3.is_type) return (sc_iterator3*)0;
        break;
    case sc_iterator3_f_a_f:
        if (p1.is_type || !p2.is_type || p3.is_type) return (sc_iterator3*)0;
        break;
    };

    sc_iterator3 *it = g_new0(sc_iterator3, 1);

    it->params[0] = p1;
    it->params[1] = p2;
    it->params[2] = p3;

    it->type = type;
    it->time_stamp = sc_storage_get_time_stamp();

    return it;
}

void sc_iterator3_free(sc_iterator3 *it)
{
    g_free(it);
}

sc_bool sc_iterator_param_compare(sc_element *el, sc_addr addr, sc_iterator_param param)
{
    g_assert(el != 0);

    if (param.is_type)
        return el->type & param.type;
    else
        return SC_ADDR_IS_EQUAL(addr, param.addr);

    return SC_FALSE;
}

sc_bool _sc_iterator3_f_a_a_next(sc_iterator3 *it)
{
    sc_addr arc_addr;
    sc_element *el1, *el2;
    sc_element *arc_element;
    sc_type arc_type, el_type;

    el1 = el2 = arc_element = 0;
    SC_ADDR_MAKE_EMPTY(arc_addr);

    it->results[0] = it->params[0].addr;

    // try to find first output arc
    if (SC_ADDR_IS_EMPTY(it->results[1]))
    {
        el1 = sc_storage_get_element(it->params[0].addr, SC_TRUE);
        arc_addr = el1->first_out_arc;
    }else
    {
        arc_element = sc_storage_get_element(it->results[1], SC_TRUE);
        arc_addr = arc_element->arc.next_out_arc;
    }

    // trying to find output arc, that created before iterator, and wasn't deleted
    while (SC_ADDR_IS_NOT_EMPTY(arc_addr))
    {
        arc_element = sc_storage_get_element(arc_addr, SC_TRUE);
        sc_storage_get_element_type(arc_addr, &arc_type);
        sc_storage_get_element_type(arc_element->arc.end, &el_type);

        if ((arc_element->create_time_stamp < it->time_stamp) &&
            (arc_element->delete_time_stamp == 0 || arc_element->delete_time_stamp > it->time_stamp) &&
            (sc_iterator_compare_type(arc_type, it->params[1].type)) &&
            (sc_iterator_compare_type(el_type, it->params[2].type))
           )
        {
            // store founded result
            it->results[1] = arc_addr;
            it->results[2] = arc_element->arc.end;

            return SC_TRUE;
        }

        // go to next arc
        arc_addr = arc_element->arc.next_out_arc;
    }

    return SC_FALSE;
}

sc_bool _sc_iterator3_f_a_f_next(sc_iterator3 *it)
{
    sc_addr arc_addr;
    sc_element *el1, *el2;
    sc_element *arc_element;
    sc_type arc_type;

    el1 = el2 = arc_element = 0;
    SC_ADDR_MAKE_EMPTY(arc_addr);

    it->results[0] = it->params[0].addr;
    it->results[2] = it->params[2].addr;

    // try to find first output arc
    if (SC_ADDR_IS_EMPTY(it->results[1]))
    {
        el1 = sc_storage_get_element(it->params[0].addr, SC_TRUE);
        arc_addr = el1->first_out_arc;
    }else
    {
        arc_element = sc_storage_get_element(it->results[1], SC_TRUE);
        arc_addr = arc_element->arc.next_out_arc;
    }

    // trying to find output arc, that created before iterator, and wasn't deleted
    while (SC_ADDR_IS_NOT_EMPTY(arc_addr))
    {
        arc_element = sc_storage_get_element(arc_addr, SC_TRUE);
        sc_storage_get_element_type(arc_addr, &arc_type);

        if ((arc_element->create_time_stamp < it->time_stamp) &&
            (arc_element->delete_time_stamp == 0 || arc_element->delete_time_stamp > it->time_stamp) &&
            SC_ADDR_IS_EQUAL(it->params[2].addr, arc_element->arc.end) &&
            (sc_iterator_compare_type(arc_type, it->params[1].type))
           )
        {
            // store founded result
            it->results[1] = arc_addr;
            return SC_TRUE;
        }

        // go to next arc
        arc_addr = arc_element->arc.next_out_arc;
    }

    return SC_FALSE;
}

sc_bool _sc_iterator3_a_a_f_next(sc_iterator3 *it)
{
    sc_addr arc_addr;
    sc_element *el1, *el2;
    sc_element *arc_element;
    sc_type arc_type, el_type;

    el1 = el2 = arc_element = 0;
    SC_ADDR_MAKE_EMPTY(arc_addr);

    it->results[2] = it->params[2].addr;

    // try to find first input arc
    if (SC_ADDR_IS_EMPTY(it->results[1]))
    {
        el1 = sc_storage_get_element(it->params[2].addr, SC_TRUE);
        arc_addr = el1->first_in_arc;
    }else
    {
        arc_element = sc_storage_get_element(it->results[1], SC_TRUE);
        arc_addr = arc_element->arc.next_in_arc;
    }

    // trying to find input arc, that created before iterator, and wasn't deleted
    while (SC_ADDR_IS_NOT_EMPTY(arc_addr))
    {
        arc_element = sc_storage_get_element(arc_addr, SC_TRUE);
        sc_storage_get_element_type(arc_addr, &arc_type);
        sc_storage_get_element_type(arc_element->arc.begin, &el_type);

        if ((arc_element->create_time_stamp < it->time_stamp) &&
            (arc_element->delete_time_stamp == 0 || arc_element->delete_time_stamp > it->time_stamp) &&
            (sc_iterator_compare_type(arc_type, it->params[1].type)) &&
            (sc_iterator_compare_type(el_type, it->params[0].type))
            )
        {
            // store founded result
            it->results[1] = arc_addr;
            it->results[0] = arc_element->arc.begin;

            return SC_TRUE;
        }

        // go to next arc
        arc_addr = arc_element->arc.next_in_arc;
    }

    return SC_FALSE;
}

sc_bool sc_iterator3_next(sc_iterator3 *it)
{
    g_assert(it != 0);


    switch (it->type)
    {

    case sc_iterator3_f_a_a:
        return _sc_iterator3_f_a_a_next(it);

    case sc_iterator3_f_a_f:
        return _sc_iterator3_f_a_f_next(it);

    case sc_iterator3_a_a_f:
        return _sc_iterator3_a_a_f_next(it);
    };

    return SC_TRUE;
}

sc_addr sc_iterator3_value(sc_iterator3 *it, sc_uint vid)
{
    g_assert(it != 0);
    g_assert(it->results != 0);

    return it->results[vid];
}

sc_bool sc_iterator_compare_type(sc_type el_type, sc_type it_type)
{
    if ((it_type & el_type) == it_type)
         return SC_TRUE;

    return SC_FALSE;
}
