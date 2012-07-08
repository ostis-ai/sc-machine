#ifndef _sc_types_h_
#define _sc_types_h_

#include "sc_config.h"
#include <glib.h>

#define SC_ADDR_SEG_MAX G_MAXUINT16
#define SC_ADDR_OFFSET_MAX G_MAXUINT16

#define SEGMENT_SIZE G_MAXUINT16    // number of elements in segment

//! Structure to store sc-element address
struct _sc_addr
{
#if USE_NETWORK_SCALE
  guint32 net_addr;
#endif
  guint16 seg;
  guint16 offset;
  guint8 attrs; // special field, that store attributes of sc-addr
};

// sc-addr attributes 
#define SC_ADDR_ATTR_NOT_EMPTY 1
// ...
// place for 7 other attributes

//! Make sc-addr empty
#define SC_ADDR_MAKE_EMPTY(addr) {addr.attrs |= ~SC_ADDR_ATTR_NOT_EMPTY; }
//! Check if specified sc-addr is empty
#define SC_ADDR_IS_NOT_EMPTY(addr) (addr.attrs & SC_ADDR_ATTR_NOT_EMPTY)
#define SC_ADDR_IS_EMPTY(addr) (!SC_ADDR_IS_NOT_EMPTY(addr))
//! Check if two sc-addr's are equivalent
#define SC_ADDR_IS_EQUAL(addr, addr2) ((addr.seg == addr.seg) && (addr.offset == addr.offset))
#define SC_ADDR_IS_NOT_EQUAL(addr, addr2) (!SC_ADDR_IS_EQUAL(addr, addr2))

/*! Next defines help to pack local part of sc-addr (segment and offset) into int value
 * and get them back from int
 */
#define SC_ADDR_LOCAL_TO_INT(addr) (guint32)((addr.seg << 16) | (addr.offset & 0xffff))
#define SC_ADDR_LOCAL_OFFSET_FROM_INT(v) (guint16)((v) & 0x0000ffff)
#define SC_ADDR_LOCAL_SEG_FROM_INT(v) SC_ADDR_LOCAL_OFFSET_FROM_INT(v >> 16)

typedef guint16 sc_type;

//! sc-element types
#define sc_type_node 1
#define sc_type_arc 1 << 1

//! constant types
#define sc_type_const 1 << 2 
#define sc_type_var 1 << 3
#define sc_type_meta 1 << 4

//! Positive types for arcs
#define sc_type_pos 1 << 5
#define sc_type_neg 1 << 6
#define sc_type_fuz 1 << 7

//! Temporary type
#define sc_type_temp 1 << 8


typedef struct _sc_arc  sc_arc;
typedef struct _sc_content sc_content;
typedef struct _sc_arc_info sc_arc_info;
typedef struct _sc_element sc_element;
typedef struct _sc_segment sc_segment;
typedef struct _sc_addr sc_addr;
typedef struct _sc_elements_stat sc_elements_stat;
typedef struct _sc_iterator_param sc_iterator_param;
typedef struct _sc_iterator sc_iterator;


#endif
