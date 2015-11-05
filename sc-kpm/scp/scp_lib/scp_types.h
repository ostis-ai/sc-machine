/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

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
#ifndef SCP_TYPES_H
#define SCP_TYPES_H

#include "sc_memory_headers.h"

#define MAKE_DEFAULT_OPERAND_FIXED(operand) { operand.addr.seg = 0; operand.addr.offset = 0; operand.element_type = 0; operand.param_type = SCP_FIXED; operand.erase = SCP_FALSE; operand.set = SCP_FALSE; operand.operand_type=SCP_CONST;}
#define MAKE_DEFAULT_OPERAND_ASSIGN(operand) { operand.addr.seg = 0; operand.addr.offset = 0; operand.element_type = 0; operand.param_type = SCP_ASSIGN; operand.erase = SCP_FALSE; operand.set = SCP_FALSE; operand.operand_type=SCP_CONST;}
#define MAKE_DEFAULT_ARC_ASSIGN(operand) { operand.addr.seg = 0; operand.addr.offset = 0; operand.element_type = scp_type_arc_pos_const_perm; operand.param_type = SCP_ASSIGN; operand.erase = SCP_FALSE; operand.set = SCP_FALSE; operand.operand_type=SCP_CONST;}
#define MAKE_COMMON_ARC_ASSIGN(operand) { operand.addr.seg = 0; operand.addr.offset = 0; operand.element_type = scp_type_arc_common | scp_type_const; operand.param_type = SCP_ASSIGN; operand.erase = SCP_FALSE; operand.set = SCP_FALSE; operand.operand_type=SCP_CONST;}
#define MAKE_DEFAULT_NODE_ASSIGN(operand) { operand.addr.seg = 0; operand.addr.offset = 0; operand.element_type = scp_type_const | scp_type_node; operand.param_type = SCP_ASSIGN; operand.erase = SCP_FALSE; operand.set = SCP_FALSE; operand.operand_type=SCP_CONST;}

#define MAKE_SC_ADDR_HASH(elem) GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(elem))
#define MAKE_HASH(elem) GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT((elem).addr))
#define MAKE_PHASH(elem) GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT((elem)->addr))

// parameter type
enum _scp_param_type
{
    SCP_ASSIGN = 0,
    SCP_FIXED = 1
};

typedef enum _scp_param_type scp_param_type;

// operand type
enum _scp_operand_type
{
    SCP_CONST = 0,
    SCP_VAR = 1
};

typedef enum _scp_operand_type scp_operand_type;

// result types
enum _scp_result
{
    SCP_RESULT_FALSE = 0,
    SCP_RESULT_TRUE = 1,
    SCP_RESULT_ERROR
};

typedef enum _scp_result scp_result;

enum _scp_bool
{
    SCP_FALSE = 0,
    SCP_TRUE = 1
};

typedef enum _scp_bool scp_bool;

typedef sc_uint16 scp_type;
typedef sc_uint32 scp_uint32;

// scp-element types
extern scp_type scp_type_node;
extern scp_type scp_type_arc;
extern scp_type scp_type_link;
extern scp_type scp_type_edge_common;
extern scp_type scp_type_arc_common;
extern scp_type scp_type_arc_access;

// scp-element constant
extern scp_type scp_type_const;
extern scp_type scp_type_var;

// scp-element positivity
extern scp_type scp_type_arc_pos;
extern scp_type scp_type_arc_neg;
extern scp_type scp_type_arc_fuz;

// scp-element premanently
extern scp_type scp_type_arc_temp;
extern scp_type scp_type_arc_perm;

extern scp_type scp_type_arc_pos_const_perm;

extern scp_type scp_type_node_not_binary_tuple;
extern scp_type scp_type_node_struct;
extern scp_type scp_type_node_role_relation;
extern scp_type scp_type_node_norole_relation;
extern scp_type scp_type_node_not_relation;
extern scp_type scp_type_node_abstract;
extern scp_type scp_type_node_material;

//! Structure to store scp-operand
struct _scp_operand
{
    sc_addr addr;
    scp_param_type param_type;
    scp_type element_type;
    scp_operand_type operand_type;
    scp_bool erase;
    scp_bool set;
};

typedef struct _scp_operand scp_operand;

struct _scp_operand_pair
{
    scp_operand *operand1;
    scp_operand *operand2;
};

typedef struct _scp_operand_pair scp_operand_pair;

#endif // SCP_TYPES_H
