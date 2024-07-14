/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_type.hpp"

ScType constexpr ScType::EdgeUCommon(sc_type_edge_common);
ScType constexpr ScType::EdgeDCommon(sc_type_arc_common);

ScType constexpr ScType::EdgeUCommonConst(sc_type_edge_common | sc_type_const);
ScType constexpr ScType::EdgeDCommonConst(sc_type_arc_common | sc_type_const);

ScType constexpr ScType::EdgeAccess(sc_type_arc_access);
ScType constexpr ScType::EdgeAccessConstPosPerm(
    sc_type_const | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_pos);
ScType constexpr ScType::EdgeAccessConstNegPerm(
    sc_type_const | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_neg);
ScType constexpr ScType::EdgeAccessConstFuzPerm(
    sc_type_const | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_fuz);
ScType constexpr ScType::EdgeAccessConstPosTemp(
    sc_type_const | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_pos);
ScType constexpr ScType::EdgeAccessConstNegTemp(
    sc_type_const | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_neg);
ScType constexpr ScType::EdgeAccessConstFuzTemp(
    sc_type_const | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_fuz);

ScType constexpr ScType::EdgeUCommonVar(sc_type_edge_common | sc_type_var);
ScType constexpr ScType::EdgeDCommonVar(sc_type_arc_common | sc_type_var);
ScType constexpr ScType::EdgeAccessVarPosPerm(sc_type_var | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_pos);
ScType constexpr ScType::EdgeAccessVarNegPerm(sc_type_var | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_neg);
ScType constexpr ScType::EdgeAccessVarFuzPerm(sc_type_var | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_fuz);
ScType constexpr ScType::EdgeAccessVarPosTemp(sc_type_var | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_pos);
ScType constexpr ScType::EdgeAccessVarNegTemp(sc_type_var | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_neg);
ScType constexpr ScType::EdgeAccessVarFuzTemp(sc_type_var | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_fuz);

ScType constexpr ScType::Const(sc_type_const);
ScType constexpr ScType::Var(sc_type_var);

ScType constexpr ScType::Node(sc_type_node);
ScType constexpr ScType::Link(sc_type_link);
ScType constexpr ScType::Unknown;

ScType constexpr ScType::NodeConst(sc_type_node | sc_type_const);
ScType constexpr ScType::NodeVar(sc_type_node | sc_type_var);

ScType constexpr ScType::LinkConst(sc_type_link | sc_type_const);
ScType constexpr ScType::LinkVar(sc_type_link | sc_type_var);
ScType constexpr ScType::LinkClass(sc_type_link | sc_type_node_class);
ScType constexpr ScType::LinkConstClass(sc_type_link | sc_type_const | sc_type_node_class);
ScType constexpr ScType::LinkVarClass(sc_type_link | sc_type_var | sc_type_node_class);

ScType constexpr ScType::NodeStruct(sc_type_node | sc_type_node_struct);
ScType constexpr ScType::NodeTuple(sc_type_node | sc_type_node_tuple);
ScType constexpr ScType::NodeRole(sc_type_node | sc_type_node_role);
ScType constexpr ScType::NodeNoRole(sc_type_node | sc_type_node_norole);
ScType constexpr ScType::NodeClass(sc_type_node | sc_type_node_class);
ScType constexpr ScType::NodeAbstract(sc_type_node | sc_type_node_abstract);
ScType constexpr ScType::NodeMaterial(sc_type_node | sc_type_node_material);

ScType constexpr ScType::NodeConstStruct(sc_type_node | sc_type_const | sc_type_node_struct);
ScType constexpr ScType::NodeConstTuple(sc_type_node | sc_type_const | sc_type_node_tuple);
ScType constexpr ScType::NodeConstRole(sc_type_node | sc_type_const | sc_type_node_role);
ScType constexpr ScType::NodeConstNoRole(sc_type_node | sc_type_const | sc_type_node_norole);
ScType constexpr ScType::NodeConstClass(sc_type_node | sc_type_const | sc_type_node_class);
ScType constexpr ScType::NodeConstAbstract(sc_type_node | sc_type_const | sc_type_node_abstract);
ScType constexpr ScType::NodeConstMaterial(sc_type_node | sc_type_const | sc_type_node_material);

ScType constexpr ScType::NodeVarStruct(sc_type_node | sc_type_var | sc_type_node_struct);
ScType constexpr ScType::NodeVarTuple(sc_type_node | sc_type_var | sc_type_node_tuple);
ScType constexpr ScType::NodeVarRole(sc_type_node | sc_type_var | sc_type_node_role);
ScType constexpr ScType::NodeVarNoRole(sc_type_node | sc_type_var | sc_type_node_norole);
ScType constexpr ScType::NodeVarClass(sc_type_node | sc_type_var | sc_type_node_class);
ScType constexpr ScType::NodeVarAbstract(sc_type_node | sc_type_var | sc_type_node_abstract);
ScType constexpr ScType::NodeVarMaterial(sc_type_node | sc_type_var | sc_type_node_material);
