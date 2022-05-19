/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_type.hpp"

ScType const ScType::EdgeUCommon(sc_type_edge_common);
ScType const ScType::EdgeDCommon(sc_type_arc_common);

ScType const ScType::EdgeUCommonConst(sc_type_edge_common | sc_type_const);
ScType const ScType::EdgeDCommonConst(sc_type_arc_common | sc_type_const);

ScType const ScType::EdgeAccess(sc_type_arc_access);
ScType const ScType::EdgeAccessConstPosPerm(sc_type_const | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_pos);
ScType const ScType::EdgeAccessConstNegPerm(sc_type_const | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_neg);
ScType const ScType::EdgeAccessConstFuzPerm(sc_type_const | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_fuz);
ScType const ScType::EdgeAccessConstPosTemp(sc_type_const | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_pos);
ScType const ScType::EdgeAccessConstNegTemp(sc_type_const | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_neg);
ScType const ScType::EdgeAccessConstFuzTemp(sc_type_const | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_fuz);

ScType const ScType::EdgeUCommonVar(sc_type_edge_common | sc_type_var);
ScType const ScType::EdgeDCommonVar(sc_type_arc_common | sc_type_var);
ScType const ScType::EdgeAccessVarPosPerm(sc_type_var | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_pos);
ScType const ScType::EdgeAccessVarNegPerm(sc_type_var | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_neg);
ScType const ScType::EdgeAccessVarFuzPerm(sc_type_var | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_fuz);
ScType const ScType::EdgeAccessVarPosTemp(sc_type_var | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_pos);
ScType const ScType::EdgeAccessVarNegTemp(sc_type_var | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_neg);
ScType const ScType::EdgeAccessVarFuzTemp(sc_type_var | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_fuz);

ScType const ScType::Const(sc_type_const);
ScType const ScType::Var(sc_type_var);

ScType const ScType::Node(sc_type_node);
ScType const ScType::Link(sc_type_link);
ScType const ScType::Unknown;

ScType const ScType::NodeConst(sc_type_node | sc_type_const);
ScType const ScType::NodeVar(sc_type_node | sc_type_var);

ScType const ScType::LinkConst(sc_type_link | sc_type_const);
ScType const ScType::LinkVar(sc_type_link | sc_type_var);

ScType const ScType::NodeStruct(sc_type_node | sc_type_node_struct);
ScType const ScType::NodeTuple(sc_type_node | sc_type_node_tuple);
ScType const ScType::NodeRole(sc_type_node | sc_type_node_role);
ScType const ScType::NodeNoRole(sc_type_node | sc_type_node_norole);
ScType const ScType::NodeClass(sc_type_node | sc_type_node_class);
ScType const ScType::NodeAbstract(sc_type_node | sc_type_node_abstract);
ScType const ScType::NodeMaterial(sc_type_node | sc_type_node_material);

ScType const ScType::NodeConstStruct(sc_type_node | sc_type_const | sc_type_node_struct);
ScType const ScType::NodeConstTuple(sc_type_node | sc_type_const | sc_type_node_tuple);
ScType const ScType::NodeConstRole(sc_type_node | sc_type_const | sc_type_node_role);
ScType const ScType::NodeConstNoRole(sc_type_node | sc_type_const | sc_type_node_norole);
ScType const ScType::NodeConstClass(sc_type_node | sc_type_const | sc_type_node_class);
ScType const ScType::NodeConstAbstract(sc_type_node | sc_type_const | sc_type_node_abstract);
ScType const ScType::NodeConstMaterial(sc_type_node | sc_type_const | sc_type_node_material);

ScType const ScType::NodeVarStruct(sc_type_node | sc_type_var | sc_type_node_struct);
ScType const ScType::NodeVarTuple(sc_type_node | sc_type_var | sc_type_node_tuple);
ScType const ScType::NodeVarRole(sc_type_node | sc_type_var | sc_type_node_role);
ScType const ScType::NodeVarNoRole(sc_type_node | sc_type_var | sc_type_node_norole);
ScType const ScType::NodeVarClass(sc_type_node | sc_type_var | sc_type_node_class);
ScType const ScType::NodeVarAbstract(sc_type_node | sc_type_var | sc_type_node_abstract);
ScType const ScType::NodeVarMaterial(sc_type_node | sc_type_var | sc_type_node_material);
