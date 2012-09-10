#ifndef _sc_types_h_
#define _sc_types_h_

#include "sc_config.h"

// base types
typedef signed char sc_int8;
typedef unsigned char sc_uint8;
typedef signed short sc_int16;
typedef unsigned short sc_uint16;
typedef signed int sc_int32;
typedef unsigned int sc_uint32;
typedef sc_uint32 sc_uint;
typedef char sc_char;
typedef unsigned char sc_uchar;

// booleans
enum _sc_bool
{
    SC_FALSE = 0,
    SC_TRUE = 1
};

typedef enum _sc_bool sc_bool;

// types limits
#define SC_MININT8	((sc_int8)  0x80)
#define SC_MAXINT8	((sc_int8)  0x7f)
#define SC_MAXUINT8	((sc_uint8) 0xff)

#define SC_MININT16     ((sc_int16)  0x8000)
#define SC_MAXINT16     ((sc_int16)  0x7fff)
#define SC_MAXUINT16	((sc_uint16) 0xffff)

#define SC_MININT32     ((sc_int32)  0x80000000)
#define SC_MAXINT32     ((sc_int32)  0x7fffffff)
#define SC_MAXUINT32	((sc_uint32) 0xffffffff)

#define SC_ADDR_SEG_MAX     SC_MAXUINT16
#define SC_ADDR_OFFSET_MAX  SC_MAXUINT16

#define SEGMENT_SIZE        SC_MAXUINT16    // number of elements in segment

// Types for segment and offset
typedef sc_uint16 sc_addr_seg;
typedef sc_uint16 sc_addr_offset;

//! Structure to store sc-element address
struct _sc_addr
{
#if USE_NETWORK_SCALE
    sc_uint32 net_addr;
#endif
    sc_addr_seg seg;
    sc_addr_offset offset;
};

//! Make sc-addr empty
#define SC_ADDR_MAKE_EMPTY(addr) { (addr).seg = 0; (addr).offset = 0; }
//! Check if specified sc-addr is empty
#define SC_ADDR_IS_EMPTY(addr) (((addr).seg == 0) && ((addr).offset == 0))
#define SC_ADDR_IS_NOT_EMPTY(addr) (!SC_ADDR_IS_EMPTY(addr))
//! Check if two sc-addr's are equivalent
#define SC_ADDR_IS_EQUAL(addr, addr2) (((addr).seg == (addr2).seg) && ((addr).offset == (addr2).offset))
#define SC_ADDR_IS_NOT_EQUAL(addr, addr2) (!SC_ADDR_IS_EQUAL(addr, addr2))

/*! Next defines help to pack local part of sc-addr (segment and offset) into int value
 * and get them back from int
 */
#define SC_ADDR_LOCAL_TO_INT(addr) (sc_uint32)((addr.seg << 16) | (addr.offset & 0xffff))
#define SC_ADDR_LOCAL_OFFSET_FROM_INT(v) (sc_uint16)((v) & 0x0000ffff)
#define SC_ADDR_LOCAL_SEG_FROM_INT(v) SC_ADDR_LOCAL_OFFSET_FROM_INT(v >> 16)

typedef sc_uint16 sc_type;

// sc-element types
#define sc_type_node        0x1
#define sc_type_link        0x2
#define sc_type_edge_common 0x4
#define sc_type_arc_common  0x8
#define sc_type_arc_access  0x10

// sc-element constant
#define sc_type_const       0x20
#define sc_type_var         0x40

// sc-element positivity
#define sc_type_arc_pos         0x80
#define sc_type_arc_neg         0x100
#define sc_type_arc_fuz         0x200

// sc-element premanently
#define sc_type_arc_temp        0x400
#define sc_type_arc_perm        0x800

// struct node types
#define sc_type_node_tuple       0x80
#define sc_type_node_struct      0x100
#define sc_type_node_role        0x200
#define sc_type_node_norole      0x400
#define sc_type_node_class       0x800
#define sc_type_node_abstract    0x1000
#define sc_type_node_material    0x2000

#define sc_type_arc_pos_const_perm (sc_type_arc_access | sc_type_const | sc_type_arc_pos | sc_type_arc_perm)

// type mask
#define sc_type_element_mask     (sc_type_node | sc_type_link | sc_type_edge_common | sc_type_arc_common | sc_type_arc_access)
#define sc_type_constancy_mask   (sc_type_const | sc_type_var)
#define sc_type_positivity_mask  (sc_type_arc_pos | sc_type_arc_neg | sc_type_arc_fuz)
#define sc_type_permanency_mask  (sc_type_arc_perm | sc_type_arc_temp)
#define sc_type_node_struct_mask (sc_type_node_tuple | sc_type_node_struct | sc_type_node_role | sc_type_node_norole | sc_type_node_class | sc_type_node_abstract | sc_type_node_material)
#define sc_type_arc_mask         (sc_type_arc_access | sc_type_arc_common | sc_type_edge_common)

// results
enum _sc_result
{
    SC_ERROR = 0,
    SC_OK = 1,
    SC_INVALID_PARAMS,
    SC_INVALID_TYPE,
    SC_IO_ERROR
};

// contents
#define SC_MAX_CHECKSUM_LEN     32
//! Structure to store checksum informaiton
struct _sc_check_sum
{
    char data[SC_MAX_CHECKSUM_LEN];  // maximum checksum length
    sc_uint8 len;    // checksum length
};

typedef struct _sc_check_sum sc_check_sum;


typedef struct _sc_arc  sc_arc;
typedef struct _sc_content sc_content;
typedef struct _sc_arc_info sc_arc_info;
typedef struct _sc_element sc_element;
typedef struct _sc_segment sc_segment;
typedef struct _sc_addr sc_addr;
typedef struct _sc_elements_stat sc_elements_stat;
typedef struct _sc_iterator_param sc_iterator_param;
typedef struct _sc_iterator3 sc_iterator3;
typedef enum _sc_result sc_result;


#endif
