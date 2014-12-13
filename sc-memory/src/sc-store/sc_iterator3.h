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

#ifndef _sc_iterator3_h_
#define _sc_iterator3_h_

#include "sc_defines.h"
#include "sc_types.h"
#include "sc_element.h"


//! sc-iterator types
typedef enum
{
    sc_iterator3_f_a_a = 0, // output arcs
    sc_iterator3_a_a_f,     // input arcs
    sc_iterator3_f_a_f      // arcs between two elements
} sc_iterator3_type;

/*! Iterator parameter
 */
struct _sc_iterator_param
{
    sc_bool is_type;
    union
    {
        sc_addr addr;
        sc_type type;
    };
};

/*! Structure to store iterator information
 */
struct _sc_iterator3
{
    sc_iterator3_type type;          // iterator type (search template)
    sc_iterator_param params[3];    // parameters array
    sc_addr results[3];             // results array (same size as params)
    const sc_memory_context *ctx;   // pointer to used memory context
    sc_bool finished;
};

/*! Create iterator to find output arcs for specified element
 * @param el sc-addr of element to iterate output arcs
 * @param arc_type Type of output arc to iterate (0 - all types)
 * @param end_type Type of end element for output arcs, to iterate
 * @return If iterator created, then return pointer to it; otherwise return null
 */
sc_iterator3* sc_iterator3_f_a_a_new(const sc_memory_context *ctx, sc_addr el, sc_type arc_type, sc_type end_type);

/*! Create iterator to find input arcs for specified element
 * @param beg_type Type of begin element for input arcs, to iterate
 * @param arc_type Type of input arc to iterate (0 - all types)
 * @param el sc-addr of element to iterate input arcs
 * @return If iterator created, then return pointer to it; otherwise return null
 */
sc_iterator3* sc_iterator3_a_a_f_new(const sc_memory_context *ctx, sc_type beg_type, sc_type arc_type, sc_addr el);

/*! Create iterator to find arcs between two specified elements
 * @param el_beg sc-addr of begin element
 * @param arc_type Type of arcs to iterate (0 - all types)
 * @param el_end sc-addr of end element
 * @return If iterator created, then return pointer to it; otherwise return null
 */
sc_iterator3* sc_iterator3_f_a_f_new(const sc_memory_context *ctx, sc_addr el_beg, sc_type arc_type, sc_addr el_end);

/*! Create new sc-iterator-3
 * @param type Iterator type (search template)
 * @param p1 First iterator parameter
 * @param p2 Second iterator parameter
 * @param p3 Third iterator parameter
 * @return Pointer to created iterator. If parameters invalid for specified iterator type, or type is not a sc-iterator-3, then return 0
 */
sc_iterator3* sc_iterator3_new(const sc_memory_context *ctx, sc_iterator3_type type, sc_iterator_param p1, sc_iterator_param p2, sc_iterator_param p3);

/*! Destroy iterator and free allocated memory
 * @param it Pointer to sc-iterator that need to be destroyed
 */
void sc_iterator3_free(sc_iterator3 *it);

/*! Go to next iterator result
 * @param it Pointer to iterator that we need to go next result
 * @return Return SC_TRUE, if iterator moved to new results; otherwise return SC_FALSE.
 * example: while(sc_iterator_next(it)) { <your code> }
 */
sc_bool sc_iterator3_next(sc_iterator3 *it);

/*! Get iterator value
 * @param it Pointer to iterator for getting value
 * @param vid Value id (can't be more that 3 for sc-iterator3)
 * @return Return sc-addr of search result value
 */
sc_addr sc_iterator3_value(sc_iterator3 *it, sc_uint vid);

/*! Check if specified element type passed into
 * iterator selection.
 * @param el_type Compared element type
 * @param it_type Template type from iterator parameter
 * @return If el_type passed checking, then return SC_TRUE, else return SC_FALSE
 */
sc_bool sc_iterator_compare_type(sc_type el_type, sc_type it_type);

#endif
