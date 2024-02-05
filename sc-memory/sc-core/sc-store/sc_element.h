/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_element_h_
#define _sc_element_h_

#include "sc_types.h"
#include "sc_defines.h"

struct _sc_arc_info
{
  sc_addr begin;
  sc_addr end;
  sc_addr next_begin_out_arc;
  sc_addr prev_begin_out_arc;
  sc_addr next_begin_in_arc;
  sc_addr prev_begin_in_arc;
  sc_addr next_end_out_arc;
  sc_addr prev_end_out_arc;
  sc_addr next_end_in_arc;
  sc_addr prev_end_in_arc;
};

/* Structure to store information for sc-elements.
 * It used just for unify working with node and arc elements.
 * When you get that structure, you need to check it type and
 * cast to sc_node or sc_arc structure depending on type.
 *
 * Each element identifier contains two numbers:
 * segment identifier and element identifier. Element
 * identifier is unique for specified segment.
 * Node also contains information about const and struct types.
 * It stores as a one char for each of types.
 * Each node have a pointer to content assigned with it. If pointer is null,
 * then node haven't any content data.
 * Last part of node information is a pointers to the lists of input and output arcs.
 * Each list of arcs contains pointer to array of arcs pointers.
 * Size of each array is fixed and equivalent to ARC_SEG_SIZE value.
 *
 * All arcs have next_arc and prev_arc addr's. Each element store addr of begin and end arcs.
 * Arc values: next_begin_out_arc and next_end_in_arc store next arcs in output and input arcs list.
 */

struct _sc_element_flags
{
  sc_type type;
  sc_states states;
};

struct _sc_element
{
  sc_element_flags flags;

  sc_addr first_out_arc;
  sc_addr first_in_arc;

  sc_arc_info arc;

  sc_uint32 input_arcs_count;
  sc_uint32 output_arcs_count;
};

#endif
