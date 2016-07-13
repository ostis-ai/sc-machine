#include "sc_types.hpp"

const ScType ScType::EDGE_ACCESS_CONST_POS_PERM(sc_type_arc_pos_const_perm);
const ScType ScType::EDGE_ACCESS_VAR_POS_PERM(sc_type_var | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_pos);



const ScType ScType::NODE_CONST(sc_type_node | sc_type_const);
const ScType ScType::NODE_VAR(sc_type_node | sc_type_var);

const ScType ScType::NODE_CONST_STRUCT(sc_type_node | sc_type_const | sc_type_node_struct);
const ScType ScType::NODE_CONST_TUPLE(sc_type_node | sc_type_const | sc_type_node_tuple);
const ScType ScType::NODE_CONST_ROLE(sc_type_node | sc_type_const | sc_type_node_role);
const ScType ScType::NODE_CONST_NOROLE(sc_type_node | sc_type_const | sc_type_node_norole);
const ScType ScType::NODE_CONST_CLASS(sc_type_node | sc_type_const | sc_type_node_class);
const ScType ScType::NODE_CONST_ABSTRACT(sc_type_node | sc_type_const | sc_type_node_abstract);
const ScType ScType::NODE_CONST_MATERIAL(sc_type_node | sc_type_const | sc_type_node_material);

const ScType ScType::NODE_VAR_STRUCT(sc_type_node | sc_type_var | sc_type_node_struct);
const ScType ScType::NODE_VAR_TUPLE(sc_type_node | sc_type_var | sc_type_node_tuple);
const ScType ScType::NODE_VAR_ROLE(sc_type_node | sc_type_var | sc_type_node_role);
const ScType ScType::NODE_VAR_NOROLE(sc_type_node | sc_type_var | sc_type_node_norole);
const ScType ScType::NODE_VAR_CLASS(sc_type_node | sc_type_var | sc_type_node_class);
const ScType ScType::NODE_VAR_ABSTRACT(sc_type_node | sc_type_var | sc_type_node_abstract);
const ScType ScType::NODE_VAR_MATERIAL(sc_type_node | sc_type_var | sc_type_node_material);
