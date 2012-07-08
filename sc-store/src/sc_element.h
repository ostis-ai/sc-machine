#ifndef _sc_element_h_
#define _sc_element_h_

#include <glib.h>
#include "sc_types.h"
#include "sc_defines.h"

struct _sc_arc_info
{
  sc_addr begin;
  sc_addr end;
  sc_addr next_out_arc; // sc-addr of next output arc in list
  sc_addr next_in_arc; // sc-addr of next input arc in list
#if USE_TWO_ORIENTED_ARC_LIST
  sc_addr prev_out_arc; // sc-addr of pevious output arc in list
  sc_addr prev_in_arc; // sc-addr of previous input arc in list
#endif
};

#if USE_TWO_ORIENTED_ARC_LIST
#define CONTENT_DATA_LEN (sizeof(sc_arc_info) - sizeof(guint32))
#else
#define CONTENT_DATA_LEN 16
#endif


//! Structure to store content information
//typedef struct _sc_content sc_content;
struct _sc_content
{
  char path[CONTENT_DATA_LEN];
  guint32 size;
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
 * It store as a one char for each of types.
 * Each node have a pointer to content assigned with it. If pointer is null,
 * then node haven't any content data.
 * Last part of node information is a pointers to the lists of input and output arcs.
 * Each list of arcs contains pointer to array of arcs pointers.
 * Size of each array is fixed and equivalent to ARC_SEG_SIZE value.
 * 
 * All arcs have next_arc and prev_arc addr's. Each element store addr of begin and end arcs.
 * Arcs values: next_out_arc and next_in_arc store next arcs in output and input arcs list.
 * So if you need to iterate all output arcs for specified element, then you need to use such code:
 * sc_element *arc = sc_storage_get_element(el->first_output_arc);
 * while (!addr_empty(arc->incident->next_out_arc))
 * {
 *     <you code>
 *     arc = sc_storage_get_element(arc->incident->next_out_arc);
 * }
 */
//typedef struct _sc_elment sc_element;
struct _sc_element
{
  sc_type type; // sc-element type
  guint32 create_time_stamp;
  guint32 delete_time_stamp;

  sc_addr first_out_arc;
  sc_addr first_in_arc;
  guint32 out_count;
  guint32 in_count;
  union
  {
    sc_content content;
    sc_arc_info arc;
  };
};

void sc_element_set_type(sc_element *element,
			 sc_type type);


#endif
