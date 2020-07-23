> TODO: Describe types there

> TODO: Types compare during search

<table>
  <tr>
    <th>Graphical (SCg)</th>
    <th>C</th>
    <th>C++</th>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node.png"></img></td>
    <td>sc_type_node</td>
    <td>ScType::Node</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const.png"></img></td>
    <td>sc_type_node | sc_type_const</td>
    <td>ScType::NodeConst</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var.png"></img></td>
    <td>sc_type_node | sc_type_var</td>
    <td>ScType::NodeVar</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const_tuple.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_tuple</td>
    <td>ScType::NodeConstTuple</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var_tuple.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_tuple</td>
    <td>ScType::NodeVarTuple</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const_struct.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_struct</td>
    <td>ScType::NodeConstStruct</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var_struct.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_struct</td>
    <td>ScType::NodeVarStruct</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const_role.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_role</td>
    <td>ScType::NodeConstRole</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var_role.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_role</td>
    <td>ScType::NodeVarRole</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const_norole.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_norole</td>
    <td>ScType::NodeConstNorole</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var_norole.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_norole</td>
    <td>ScType::NodeVarNorole</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const_class.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_class</td>
    <td>ScType::NodeConstClass</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var_class.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_class</td>
    <td>ScType::NodeVarClass</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const_abstract.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_abstract</td>
    <td>ScType::NodeConstAbstract</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var_abstract.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_abstract</td>
    <td>ScType::NodeVarAbstract</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const_material.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_material</td>
    <td>ScType::NodeConstMaterial</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var_material.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_material</td>
    <td>ScType::NodeVarMaterial</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_common.png"></img></td>
    <td>sc_type_edge_common</td>
    <td>ScType::EdgeUCommon</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_common_orient.png"></img></td>
    <td>sc_type_arc_common</td>
    <td>ScType::EdgeDCommon</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_common.png"></img></td>
    <td>sc_type_edge_common | sc_type_const</td>
    <td>ScType::EdgeUCommonConst</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_common.png"></img></td>
    <td>sc_type_edge_common | sc_type_var</td>
    <td>ScType::EdgeUCommonVar</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_common_orient.png"></img></td>
    <td>sc_type_arc_common | sc_type_const</td>
    <td>ScType::EdgeDCommonConst</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_common_orient.png"></img></td>
    <td>sc_type_arc_common | sc_type_var</td>
    <td>ScType::EdgeDCommonVar</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_access.png"></img></td>
    <td>sc_type_arc_access</td>
    <td>ScType::EdgeAccess</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_pos_perm.png"></img></td>
    <td>sc_type_arc_access | sc_type_const | sc_type_arc_pos | sc_type_arc_perm</td>
    <td>ScType::EdgeAccessConstPosPerm</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_pos_perm.png"></img></td>
    <td>sc_type_arc_access | sc_type_var | sc_type_arc_pos | sc_type_arc_perm</td>
    <td>ScType::EdgeAccessVarPosPerm</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_neg_perm.png"></img></td>
    <td>sc_type_arc_access | sc_type_const | sc_type_arc_neg | sc_type_arc_perm</td>
    <td>ScType::EdgeAccessConstNegPerm</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_neg_perm.png"></img></td>
    <td>sc_type_arc_access | sc_type_var | sc_type_arc_neg | sc_type_arc_perm</td>
    <td>ScType::EdgeAccessVarNegPerm</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_fuz_perm.png"></img></td>
    <td>sc_type_arc_access | sc_type_const | sc_type_arc_fuz | sc_type_arc_perm</td>
    <td>ScType::EdgeAccessConstFuzPerm</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_fuz_perm.png"></img></td>
    <td>sc_type_arc_access | sc_type_var | sc_type_arc_fuz | sc_type_arc_perm</td>
    <td>ScType::EdgeAccessVarFuzPerm</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_pos_temp.png"></img></td>
    <td>sc_type_arc_access | sc_type_const | sc_type_arc_pos | sc_type_arc_temp</td>
    <td>ScType::EdgeAccessConstPosTemp</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_pos_temp.png"></img></td>
    <td>sc_type_arc_access | sc_type_var | sc_type_arc_pos | sc_type_arc_temp</td>
    <td>ScType::EdgeAccessVarPosPerm</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_neg_temp.png"></img></td>
    <td>sc_type_arc_access | sc_type_const | sc_type_arc_neg | sc_type_arc_temp</td>
    <td>ScType::EdgeAccessConstNegTemp</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_neg_temp.png"></img></td>
    <td>sc_type_arc_access | sc_type_var | sc_type_arc_neg | sc_type_arc_temp</td>
    <td>ScType::EdgeAccessVarNegPerm</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_fuz_temp.png"></img></td>
    <td>sc_type_arc_access | sc_type_const | sc_type_arc_fuz | sc_type_arc_temp</td>
    <td>ScType::EdgeAccessConstFuzTemp</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_fuz_temp.png"></img></td>
    <td>sc_type_arc_access | sc_type_var | sc_type_arc_fuz | sc_type_arc_temp</td>
    <td>ScType::EdgeAccessVarFuzPerm</td>
  </tr>

</table>
