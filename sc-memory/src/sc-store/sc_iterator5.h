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

#ifndef _sc_iterator5_h_
#define _sc_iterator5_h_

#include "sc_iterator3.h"

//! sc-iterator5 types
typedef enum
{
    sc_iterator5_f_a_a_a_f = 0,
    sc_iterator5_a_a_f_a_f,
    sc_iterator5_f_a_f_a_f,
    sc_iterator5_f_a_f_a_a,
    sc_iterator5_f_a_a_a_a,
    sc_iterator5_a_a_f_a_a
} sc_iterator5_type;

/*! Structure to store iterator information
 */
struct _sc_iterator5
{
    sc_iterator5_type type;         // iterator type (search template)
    sc_iterator_param params[5];    // parameters array
    sc_addr results[5];             // results array (same size as params)
    sc_iterator3* it_main;          // iterator of main arc
    sc_iterator3* it_attr;          // iterator of attribute arc
    sc_uint32 time_stamp;           // iterator time stamp
    const sc_memory_context *ctx;   // pointer to used memory context
};

typedef struct _sc_iterator5 sc_iterator5;

/*! Create new sc-iterator5
 * @param type Iterator type (search template)
 * @param p1 First element type
 * @param p2 Second element type
 * @param p3 sc-addr of third element in construction
 * @param p4 4th element type
 * @param p5 sc-addr of 5th element in construction
 * @return Pointer to created iterator. If parameters invalid for specified iterator type, or type is not a sc-iterator-3, then return 0
 */
sc_iterator5* sc_iterator5_a_a_f_a_f_new(const sc_memory_context *ctx, sc_type p1, sc_type p2, sc_addr p3, sc_type p4, sc_addr p5);

/*! Create new sc-iterator5
 * @param type Iterator type (search template)
 * @param p1 sc-addr of first element in construction
 * @param p2 Second element type
 * @param p3 Third element type
 * @param p4 4-th element type
 * @param p5 sc-addr of 5th element in construction
 * @return Pointer to created iterator. If parameters invalid for specified iterator type, or type is not a sc-iterator-3, then return 0
 */
sc_iterator5* sc_iterator5_f_a_a_a_f_new(const sc_memory_context *ctx, sc_addr p1, sc_type p2, sc_type p3, sc_type p4, sc_addr p5);

/*! Create new sc-iterator5
 * @param type Iterator type (search template)
 * @param p1 sc-addr of first element in construction
 * @param p2 Second element type
 * @param p3 sc-addr of third element in construction
 * @param p4 4-th element type
 * @param p5 sc-addr of 5th element in construction
 * @return Pointer to created iterator. If parameters invalid for specified iterator type, or type is not a sc-iterator-3, then return 0
 */
sc_iterator5* sc_iterator5_f_a_f_a_f_new(const sc_memory_context *ctx, sc_addr p1, sc_type p2, sc_addr p3, sc_type p4, sc_addr p5);

/*! Create new sc-iterator5
 * @param type Iterator type (search template)
 * @param p1 sc-addr of first element in construction
 * @param p2 Second element type
 * @param p3 sc-addr of first element in construction
 * @param p4 4-th element type
 * @param p5 5-th element type
 * @return Pointer to created iterator. If parameters invalid for specified iterator type, or type is not a sc-iterator-3, then return 0
 */
sc_iterator5* sc_iterator5_f_a_f_a_a_new(const sc_memory_context *ctx, sc_addr p1, sc_type p2, sc_addr p3, sc_type p4, sc_type p5);

/*! Create new sc-iterator5
 * @param type Iterator type (search template)
 * @param p1 sc-addr of first element in construction
 * @param p2 Second element type
 * @param p3 Third element type
 * @param p4 4-th element type
 * @param p5 5-th element type
 * @return Pointer to created iterator. If parameters invalid for specified iterator type, or type is not a sc-iterator-3, then return 0
 */
sc_iterator5* sc_iterator5_f_a_a_a_a_new(const sc_memory_context *ctx, sc_addr p1, sc_type p2, sc_type p3, sc_type p4, sc_type p5);

/*! Create new sc-iterator5
 * @param type Iterator type (search template)
 * @param p1 First element type
 * @param p2 Second element type
 * @param p3 sc-addr of third element in construction
 * @param p4 4-th element type
 * @param p5 5-th element type
 * @return Pointer to created iterator. If parameters invalid for specified iterator type, or type is not a sc-iterator-3, then return 0
 */
sc_iterator5* sc_iterator5_a_a_f_a_a_new(const sc_memory_context *ctx, sc_type p1, sc_type p2, sc_addr p3, sc_type p4, sc_type p5);

/*! Go to next iterator result
 * @param it Pointer to iterator that we need to go next result
 * @return Return SC_TRUE, if iterator moved to new results; otherwise return SC_FALSE.
 * example: while(sc_iterator_next(it)) { <your code> }
 */
sc_bool sc_iterator5_next(sc_iterator5 *it);

/*! Get iterator value
 * @param it Pointer to iterator for getting value
 * @param vid Value id (can't be more that 5 for sc-iterator5)
 * @return Return sc-addr of search result value
 */
sc_addr sc_iterator5_value(sc_iterator5 *it, sc_uint vid);

/*! Destroy iterator and free allocated memory
 * @param it Pointer to sc-iterator that need to be destroyed
 */
void sc_iterator5_free(sc_iterator5 *it);

#endif // SC_ITERATOR5_H
