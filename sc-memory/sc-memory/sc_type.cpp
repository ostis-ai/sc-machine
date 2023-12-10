/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_type.hpp"

ScType const ScType::Unknown;

ScType const ScType::Const(sc_type_const);
ScType const ScType::Var(sc_type_var);
ScType const ScType::MetaVar(sc_type_metavar);

ScType const ScType::Perm(sc_type_perm);
ScType const ScType::Temp(sc_type_temp);

ScType const ScType::Node(sc_type_node);

ScType const ScType::NodeConst(sc_type_node | sc_type_const);
ScType const ScType::NodeVar(sc_type_node | sc_type_var);

ScType const ScType::NodePerm(sc_type_node | sc_type_perm);
ScType const ScType::NodeTemp(sc_type_node | sc_type_temp);

ScType const ScType::NodeStruct(sc_type_node | sc_type_node_struct);
ScType const ScType::NodeTuple(sc_type_node | sc_type_node_tuple);
ScType const ScType::NodeRole(sc_type_node | sc_type_node_role);
ScType const ScType::NodeNoRole(sc_type_node | sc_type_node_norole);
ScType const ScType::NodeClass(sc_type_node | sc_type_node_class);
ScType const ScType::NodeSuperClass(sc_type_node | sc_type_node_superclass);
ScType const ScType::NodeAbstract(sc_type_node | sc_type_node_abstract);
ScType const ScType::NodeMaterial(sc_type_node | sc_type_node_material);
ScType const ScType::Link(sc_type_node | sc_type_link);
ScType const ScType::LinkClass(sc_type_node | sc_type_link | sc_type_link_class);

ScType const ScType::NodeConstStruct(sc_type_node | sc_type_const | sc_type_node_struct);
ScType const ScType::NodeConstTuple(sc_type_node | sc_type_const | sc_type_node_tuple);
ScType const ScType::NodeConstRole(sc_type_node | sc_type_const | sc_type_node_role);
ScType const ScType::NodeConstNoRole(sc_type_node | sc_type_const | sc_type_node_norole);
ScType const ScType::NodeConstClass(sc_type_node | sc_type_const | sc_type_node_class);
ScType const ScType::NodeConstSuperClass(sc_type_node | sc_type_const | sc_type_node_superclass);
ScType const ScType::NodeConstAbstract(sc_type_node | sc_type_const | sc_type_node_abstract);
ScType const ScType::NodeConstMaterial(sc_type_node | sc_type_const | sc_type_node_material);
ScType const ScType::LinkConst(sc_type_node | sc_type_const | sc_type_link);
ScType const ScType::LinkConstClass(sc_type_node | sc_type_const | sc_type_link | sc_type_link_class);

ScType const ScType::NodeVarStruct(sc_type_node | sc_type_var | sc_type_node_struct);
ScType const ScType::NodeVarTuple(sc_type_node | sc_type_var | sc_type_node_tuple);
ScType const ScType::NodeVarRole(sc_type_node | sc_type_var | sc_type_node_role);
ScType const ScType::NodeVarNoRole(sc_type_node | sc_type_var | sc_type_node_norole);
ScType const ScType::NodeVarClass(sc_type_node | sc_type_var | sc_type_node_class);
ScType const ScType::NodeVarSuperClass(sc_type_node | sc_type_var | sc_type_node_superclass);
ScType const ScType::NodeVarAbstract(sc_type_node | sc_type_var | sc_type_node_abstract);
ScType const ScType::NodeVarMaterial(sc_type_node | sc_type_var | sc_type_node_material);
ScType const ScType::LinkVar(sc_type_node | sc_type_var | sc_type_link);
ScType const ScType::LinkVarClass(sc_type_node | sc_type_var | sc_type_link | sc_type_link_class);

ScType const ScType::NodeMetaVarStruct(sc_type_node | sc_type_metavar | sc_type_node_struct);
ScType const ScType::NodeMetaVarTuple(sc_type_node | sc_type_metavar | sc_type_node_tuple);
ScType const ScType::NodeMetaVarRole(sc_type_node | sc_type_metavar | sc_type_node_role);
ScType const ScType::NodeMetaVarNoRole(sc_type_node | sc_type_metavar | sc_type_node_norole);
ScType const ScType::NodeMetaVarClass(sc_type_node | sc_type_metavar | sc_type_node_class);
ScType const ScType::NodeMetaVarSuperClass(sc_type_node | sc_type_metavar | sc_type_node_superclass);
ScType const ScType::NodeMetaVarAbstract(sc_type_node | sc_type_metavar | sc_type_node_abstract);
ScType const ScType::NodeMetaVarMaterial(sc_type_node | sc_type_metavar | sc_type_node_material);
ScType const ScType::LinkMetaVar(sc_type_node | sc_type_metavar | sc_type_link);
ScType const ScType::LinkMetaVarClass(sc_type_node | sc_type_metavar | sc_type_link | sc_type_link_class);

ScType const ScType::NodePermStruct(sc_type_node | sc_type_perm | sc_type_node_struct);
ScType const ScType::NodePermTuple(sc_type_node | sc_type_perm | sc_type_node_tuple);
ScType const ScType::NodePermRole(sc_type_node | sc_type_perm | sc_type_node_role);
ScType const ScType::NodePermNoRole(sc_type_node | sc_type_perm | sc_type_node_norole);
ScType const ScType::NodePermClass(sc_type_node | sc_type_perm | sc_type_node_class);
ScType const ScType::NodePermSuperClass(sc_type_node | sc_type_perm | sc_type_node_superclass);
ScType const ScType::NodePermAbstract(sc_type_node | sc_type_perm | sc_type_node_abstract);
ScType const ScType::NodePermMaterial(sc_type_node | sc_type_perm | sc_type_node_material);
ScType const ScType::LinkPermConst(sc_type_node | sc_type_perm | sc_type_link);
ScType const ScType::LinkPermConstClass(sc_type_node | sc_type_perm | sc_type_link | sc_type_link_class);

ScType const ScType::NodeTempStruct(sc_type_node | sc_type_temp | sc_type_node_struct);
ScType const ScType::NodeTempTuple(sc_type_node | sc_type_temp | sc_type_node_tuple);
ScType const ScType::NodeTempRole(sc_type_node | sc_type_temp | sc_type_node_role);
ScType const ScType::NodeTempNoRole(sc_type_node | sc_type_temp | sc_type_node_norole);
ScType const ScType::NodeTempClass(sc_type_node | sc_type_temp | sc_type_node_class);
ScType const ScType::NodeTempSuperClass(sc_type_node | sc_type_temp | sc_type_node_superclass);
ScType const ScType::NodeTempAbstract(sc_type_node | sc_type_temp | sc_type_node_abstract);
ScType const ScType::NodeTempMaterial(sc_type_node | sc_type_temp | sc_type_node_material);
ScType const ScType::LinkTempConst(sc_type_node | sc_type_temp | sc_type_link);
ScType const ScType::LinkTempConstClass(sc_type_node | sc_type_temp | sc_type_link | sc_type_link_class);

ScType const ScType::NodeConstPermStruct(sc_type_node | sc_type_const | sc_type_perm | sc_type_node_struct);
ScType const ScType::NodeConstPermTuple(sc_type_node | sc_type_const | sc_type_perm | sc_type_node_tuple);
ScType const ScType::NodeConstPermRole(sc_type_node | sc_type_const | sc_type_perm | sc_type_node_role);
ScType const ScType::NodeConstPermNoRole(sc_type_node | sc_type_const | sc_type_perm | sc_type_node_norole);
ScType const ScType::NodeConstPermClass(sc_type_node | sc_type_const | sc_type_perm | sc_type_node_class);
ScType const ScType::NodeConstPermSuperClass(sc_type_node | sc_type_const | sc_type_perm | sc_type_node_superclass);
ScType const ScType::NodeConstPermAbstract(sc_type_node | sc_type_const | sc_type_perm | sc_type_node_abstract);
ScType const ScType::NodeConstPermMaterial(sc_type_node | sc_type_const | sc_type_perm | sc_type_node_material);
ScType const ScType::LinkConstPerm(sc_type_node | sc_type_const | sc_type_perm | sc_type_link);
ScType const ScType::LinkConstPermClass(
    sc_type_node | sc_type_const | sc_type_perm | sc_type_link | sc_type_link_class);

ScType const ScType::NodeVarPermStruct(sc_type_node | sc_type_var | sc_type_perm | sc_type_node_struct);
ScType const ScType::NodeVarPermTuple(sc_type_node | sc_type_var | sc_type_perm | sc_type_node_tuple);
ScType const ScType::NodeVarPermRole(sc_type_node | sc_type_var | sc_type_perm | sc_type_node_role);
ScType const ScType::NodeVarPermNoRole(sc_type_node | sc_type_var | sc_type_perm | sc_type_node_norole);
ScType const ScType::NodeVarPermClass(sc_type_node | sc_type_var | sc_type_perm | sc_type_node_class);
ScType const ScType::NodeVarPermSuperClass(sc_type_node | sc_type_var | sc_type_perm | sc_type_node_superclass);
ScType const ScType::NodeVarPermAbstract(sc_type_node | sc_type_var | sc_type_perm | sc_type_node_abstract);
ScType const ScType::NodeVarPermMaterial(sc_type_node | sc_type_var | sc_type_perm | sc_type_node_material);
ScType const ScType::LinkVarPerm(sc_type_node | sc_type_var | sc_type_perm | sc_type_link);
ScType const ScType::LinkVarPermClass(sc_type_node | sc_type_var | sc_type_perm | sc_type_link | sc_type_link_class);

ScType const ScType::NodeMetaVarPermStruct(sc_type_node | sc_type_metavar | sc_type_perm | sc_type_node_struct);
ScType const ScType::NodeMetaVarPermTuple(sc_type_node | sc_type_metavar | sc_type_perm | sc_type_node_tuple);
ScType const ScType::NodeMetaVarPermRole(sc_type_node | sc_type_metavar | sc_type_perm | sc_type_node_role);
ScType const ScType::NodeMetaVarPermNoRole(sc_type_node | sc_type_metavar | sc_type_perm | sc_type_node_norole);
ScType const ScType::NodeMetaVarPermClass(sc_type_node | sc_type_metavar | sc_type_perm | sc_type_node_class);
ScType const ScType::NodeMetaVarPermSuperClass(sc_type_node | sc_type_metavar | sc_type_perm | sc_type_node_superclass);
ScType const ScType::NodeMetaVarPermAbstract(sc_type_node | sc_type_metavar | sc_type_perm | sc_type_node_abstract);
ScType const ScType::NodeMetaVarPermMaterial(sc_type_node | sc_type_metavar | sc_type_perm | sc_type_node_material);
ScType const ScType::LinkMetaVarPerm(sc_type_node | sc_type_metavar | sc_type_perm | sc_type_link);
ScType const ScType::LinkMetaVarPermClass(
    sc_type_node | sc_type_metavar | sc_type_perm | sc_type_link | sc_type_link_class);

ScType const ScType::NodeConstTempStruct(sc_type_node | sc_type_const | sc_type_temp | sc_type_node_struct);
ScType const ScType::NodeConstTempTuple(sc_type_node | sc_type_const | sc_type_temp | sc_type_node_tuple);
ScType const ScType::NodeConstTempRole(sc_type_node | sc_type_const | sc_type_temp | sc_type_node_role);
ScType const ScType::NodeConstTempNoRole(sc_type_node | sc_type_const | sc_type_temp | sc_type_node_norole);
ScType const ScType::NodeConstTempClass(sc_type_node | sc_type_const | sc_type_temp | sc_type_node_class);
ScType const ScType::NodeConstTempSuperClass(sc_type_node | sc_type_const | sc_type_temp | sc_type_node_superclass);
ScType const ScType::NodeConstTempAbstract(sc_type_node | sc_type_const | sc_type_temp | sc_type_node_abstract);
ScType const ScType::NodeConstTempMaterial(sc_type_node | sc_type_const | sc_type_temp | sc_type_node_material);
ScType const ScType::LinkConstTemp(sc_type_node | sc_type_const | sc_type_temp | sc_type_link);
ScType const ScType::LinkConstTempClass(
    sc_type_node | sc_type_const | sc_type_temp | sc_type_link | sc_type_link_class);

ScType const ScType::NodeVarTempStruct(sc_type_node | sc_type_var | sc_type_temp | sc_type_node_struct);
ScType const ScType::NodeVarTempTuple(sc_type_node | sc_type_var | sc_type_temp | sc_type_node_tuple);
ScType const ScType::NodeVarTempRole(sc_type_node | sc_type_var | sc_type_temp | sc_type_node_role);
ScType const ScType::NodeVarTempNoRole(sc_type_node | sc_type_var | sc_type_temp | sc_type_node_norole);
ScType const ScType::NodeVarTempClass(sc_type_node | sc_type_var | sc_type_temp | sc_type_node_class);
ScType const ScType::NodeVarTempSuperClass(sc_type_node | sc_type_var | sc_type_temp | sc_type_node_superclass);
ScType const ScType::NodeVarTempAbstract(sc_type_node | sc_type_var | sc_type_temp | sc_type_node_abstract);
ScType const ScType::NodeVarTempMaterial(sc_type_node | sc_type_var | sc_type_temp | sc_type_node_material);
ScType const ScType::LinkVarTemp(sc_type_node | sc_type_var | sc_type_temp | sc_type_link);
ScType const ScType::LinkVarTempClass(sc_type_node | sc_type_var | sc_type_temp | sc_type_link | sc_type_link_class);

ScType const ScType::NodeMetaVarTempStruct(sc_type_node | sc_type_metavar | sc_type_temp | sc_type_node_struct);
ScType const ScType::NodeMetaVarTempTuple(sc_type_node | sc_type_metavar | sc_type_temp | sc_type_node_tuple);
ScType const ScType::NodeMetaVarTempRole(sc_type_node | sc_type_metavar | sc_type_temp | sc_type_node_role);
ScType const ScType::NodeMetaVarTempNoRole(sc_type_node | sc_type_metavar | sc_type_temp | sc_type_node_norole);
ScType const ScType::NodeMetaVarTempClass(sc_type_node | sc_type_metavar | sc_type_temp | sc_type_node_class);
ScType const ScType::NodeMetaVarTempSuperClass(sc_type_node | sc_type_metavar | sc_type_temp | sc_type_node_superclass);
ScType const ScType::NodeMetaVarTempAbstract(sc_type_node | sc_type_metavar | sc_type_temp | sc_type_node_abstract);
ScType const ScType::NodeMetaVarTempMaterial(sc_type_node | sc_type_metavar | sc_type_temp | sc_type_node_material);
ScType const ScType::LinkMetaVarTemp(sc_type_node | sc_type_metavar | sc_type_temp | sc_type_link);
ScType const ScType::LinkMetaVarTempClass(
    sc_type_node | sc_type_metavar | sc_type_temp | sc_type_link | sc_type_link_class);

ScType const ScType::Connector(sc_type_connector);
ScType const ScType::Arc(sc_type_connector);

ScType const ScType::EdgeCommon(sc_type_edge_common);
ScType const ScType::ArcCommon(sc_type_arc_common);
ScType const ScType::ArcAccess(sc_type_arc_access);

// backward compatibility
ScType const ScType::EdgeUCommon(sc_type_edge_common);
ScType const ScType::EdgeDCommon(sc_type_arc_common);
ScType const ScType::EdgeAccess(sc_type_arc_access);

ScType const ScType::EdgeCommonConst(sc_type_edge_common | sc_type_const);
ScType const ScType::ArcCommonConst(sc_type_arc_common | sc_type_const);
ScType const ScType::ArcAccessConst(sc_type_arc_access | sc_type_const);

// backward compatibility
ScType const ScType::EdgeUCommonConst(sc_type_edge_common | sc_type_const);
ScType const ScType::EdgeDCommonConst(sc_type_arc_common | sc_type_const);

ScType const ScType::EdgeCommonVar(sc_type_edge_common | sc_type_var);
ScType const ScType::ArcCommonVar(sc_type_arc_common | sc_type_var);
ScType const ScType::ArcAccessVar(sc_type_arc_access | sc_type_var);

// backward compatibility
ScType const ScType::EdgeUCommonVar(sc_type_edge_common | sc_type_var);
ScType const ScType::EdgeDCommonVar(sc_type_arc_common | sc_type_var);

ScType const ScType::EdgeCommonMetaVar(sc_type_edge_common | sc_type_metavar);
ScType const ScType::ArcCommonMetaVar(sc_type_arc_common | sc_type_metavar);
ScType const ScType::ArcAccessMetaVar(sc_type_arc_access | sc_type_metavar);

ScType const ScType::EdgeCommonPerm(sc_type_edge_common | sc_type_perm);
ScType const ScType::ArcCommonPerm(sc_type_arc_common | sc_type_perm);
ScType const ScType::ArcAccessPerm(sc_type_arc_access | sc_type_perm);

ScType const ScType::EdgeCommonTemp(sc_type_edge_common | sc_type_temp);
ScType const ScType::ArcCommonTemp(sc_type_arc_common | sc_type_temp);
ScType const ScType::ArcAccessTemp(sc_type_arc_access | sc_type_temp);

ScType const ScType::EdgeCommonConstPerm(sc_type_edge_common | sc_type_const | sc_type_perm);
ScType const ScType::ArcCommonConstPerm(sc_type_arc_common | sc_type_const | sc_type_perm);

ScType const ScType::EdgeCommonVarPerm(sc_type_edge_common | sc_type_var | sc_type_perm);
ScType const ScType::ArcCommonVarPerm(sc_type_arc_common | sc_type_var | sc_type_perm);

ScType const ScType::EdgeCommonMetaVarPerm(sc_type_edge_common | sc_type_metavar | sc_type_perm);
ScType const ScType::ArcCommonMetaVarPerm(sc_type_arc_common | sc_type_metavar | sc_type_perm);

ScType const ScType::EdgeCommonConstTemp(sc_type_edge_common | sc_type_const | sc_type_temp);
ScType const ScType::ArcCommonConstTemp(sc_type_arc_common | sc_type_const | sc_type_temp);

ScType const ScType::EdgeCommonVarTemp(sc_type_edge_common | sc_type_var | sc_type_temp);
ScType const ScType::ArcCommonVarTemp(sc_type_arc_common | sc_type_var | sc_type_temp);

ScType const ScType::EdgeCommonMetaVarTemp(sc_type_edge_common | sc_type_metavar | sc_type_temp);
ScType const ScType::ArcCommonMetaVarTemp(sc_type_arc_common | sc_type_metavar | sc_type_temp);

ScType const ScType::EdgeAccessPos(sc_type_arc_access | sc_type_arc_pos);
ScType const ScType::EdgeAccessNeg(sc_type_arc_access | sc_type_arc_neg);
ScType const ScType::EdgeAccessFuz(sc_type_arc_access | sc_type_arc_fuz);

ScType const ScType::EdgeAccessConstPos(sc_type_arc_access | sc_type_const | sc_type_arc_pos);
ScType const ScType::EdgeAccessConstNeg(sc_type_arc_access | sc_type_const | sc_type_arc_neg);
ScType const ScType::EdgeAccessConstFuz(sc_type_arc_access | sc_type_const | sc_type_arc_fuz);

ScType const ScType::EdgeAccessVarPos(sc_type_arc_access | sc_type_var | sc_type_arc_pos);
ScType const ScType::EdgeAccessVarNeg(sc_type_arc_access | sc_type_var | sc_type_arc_neg);
ScType const ScType::EdgeAccessVarFuz(sc_type_arc_access | sc_type_var | sc_type_arc_fuz);

ScType const ScType::EdgeAccessMetaVarPos(sc_type_arc_access | sc_type_metavar | sc_type_arc_pos);
ScType const ScType::EdgeAccessMetaVarNeg(sc_type_arc_access | sc_type_metavar | sc_type_arc_neg);
ScType const ScType::EdgeAccessMetaVarFuz(sc_type_arc_access | sc_type_metavar | sc_type_arc_fuz);

ScType const ScType::EdgeAccessConstPerm(sc_type_arc_access | sc_type_const | sc_type_arc_perm);
ScType const ScType::EdgeAccessConstTemp(sc_type_arc_access | sc_type_const | sc_type_arc_temp);

ScType const ScType::EdgeAccessVarPerm(sc_type_arc_access | sc_type_var | sc_type_arc_perm);
ScType const ScType::EdgeAccessVarTemp(sc_type_arc_access | sc_type_var | sc_type_arc_temp);

ScType const ScType::EdgeAccessMetaVarPerm(sc_type_arc_access | sc_type_metavar | sc_type_arc_perm);
ScType const ScType::EdgeAccessMetaVarTemp(sc_type_arc_access | sc_type_metavar | sc_type_arc_temp);

ScType const ScType::EdgeAccessPosPerm(sc_type_arc_access | sc_type_arc_pos | sc_type_perm);
ScType const ScType::EdgeAccessNegPerm(sc_type_arc_access | sc_type_arc_neg | sc_type_perm);
ScType const ScType::EdgeAccessFuzPerm(sc_type_arc_access | sc_type_arc_fuz | sc_type_perm);

ScType const ScType::EdgeAccessPosTemp(sc_type_arc_access | sc_type_arc_pos | sc_type_temp);
ScType const ScType::EdgeAccessNegTemp(sc_type_arc_access | sc_type_arc_neg | sc_type_temp);
ScType const ScType::EdgeAccessFuzTemp(sc_type_arc_access | sc_type_arc_fuz | sc_type_temp);

ScType const ScType::EdgeAccessConstPosPerm(sc_type_const | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_pos);
ScType const ScType::EdgeAccessConstNegPerm(sc_type_const | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_neg);
ScType const ScType::EdgeAccessConstFuzPerm(sc_type_const | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_fuz);

ScType const ScType::EdgeAccessConstPosTemp(sc_type_const | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_pos);
ScType const ScType::EdgeAccessConstNegTemp(sc_type_const | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_neg);
ScType const ScType::EdgeAccessConstFuzTemp(sc_type_const | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_fuz);

ScType const ScType::EdgeAccessVarPosPerm(sc_type_var | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_pos);
ScType const ScType::EdgeAccessVarNegPerm(sc_type_var | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_neg);
ScType const ScType::EdgeAccessVarFuzPerm(sc_type_var | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_fuz);

ScType const ScType::EdgeAccessVarPosTemp(sc_type_var | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_pos);
ScType const ScType::EdgeAccessVarNegTemp(sc_type_var | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_neg);
ScType const ScType::EdgeAccessVarFuzTemp(sc_type_var | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_fuz);

ScType const ScType::EdgeAccessMetaVarPosPerm(
    sc_type_metavar | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_pos);
ScType const ScType::EdgeAccessMetaVarNegPerm(
    sc_type_metavar | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_neg);
ScType const ScType::EdgeAccessMetaVarFuzPerm(
    sc_type_metavar | sc_type_arc_access | sc_type_arc_perm | sc_type_arc_fuz);

ScType const ScType::EdgeAccessMetaVarPosTemp(
    sc_type_metavar | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_pos);
ScType const ScType::EdgeAccessMetaVarNegTemp(
    sc_type_metavar | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_neg);
ScType const ScType::EdgeAccessMetaVarFuzTemp(
    sc_type_metavar | sc_type_arc_access | sc_type_arc_temp | sc_type_arc_fuz);
