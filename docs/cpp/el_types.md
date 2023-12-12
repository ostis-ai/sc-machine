> TODO: Describe types there

> TODO: Types compare during search

<table>
  <tr>
    <th>Graphical (SCg)</th>
    <th>C</th>
    <th>C++</th>
    <th>SCs</th>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node.png"></img></td>
    <td>sc_type_node</td>
    <td>ScType::Node</td>
    <td>sc_node</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const.png"></img></td>
    <td>sc_type_node | sc_type_const</td>
    <td>ScType::NodeConst</td>
    <td></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var.png"></img></td>
    <td>sc_type_node | sc_type_var</td>
    <td>ScType::NodeVar</td>
    <td></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const_tuple.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_tuple</td>
    <td>ScType::NodeConstTuple</td>
    <td>sc_node_tuple <br> sc_node_not_binary_tuple</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var_tuple.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_tuple</td>
    <td>ScType::NodeVarTuple</td>
    <td></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const_struct.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_struct</td>
    <td>ScType::NodeConstStruct</td>
    <td>sc_node_struct</td>
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
    <td>sc_node_role_relation</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var_role.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_role</td>
    <td>ScType::NodeVarRole</td>
    <td></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const_norole.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_norole</td>
    <td>ScType::NodeConstNorole</td>
    <td>sc_node_norole_relation</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var_norole.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_norole</td>
    <td>ScType::NodeVarNorole</td>
    <td></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const_class.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_class</td>
    <td>ScType::NodeConstClass</td>
    <td>sc_node_class <br> sc_node_not_relation</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var_class.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_class</td>
    <td>ScType::NodeVarClass</td>
    <td></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_const_material.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_material</td>
    <td>ScType::NodeConstMaterial</td>
    <td>sc_node_material</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_node_var_material.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_material</td>
    <td>ScType::NodeVarMaterial</td>
    <td></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_link_const.png"></img></td>
    <td>sc_link</td>
    <td>ScType::Link</td>
    <td>sc_link</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_common.png"></img></td>
    <td>sc_type_edge_common</td>
    <td>ScType::EdgeUCommon</td>
    <td>sc_edge<br>sc_edge_ucommon<br><></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_common_orient.png"></img></td>
    <td>sc_type_arc_common</td>
    <td>ScType::EdgeDCommon</td>
    <td>sc_arc_common <br> sc_edge_common <br> < <br> ></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_common.png"></img></td>
    <td>sc_type_edge_common | sc_type_const</td>
    <td>ScType::EdgeUCommonConst</td>
    <td><=></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_common.png"></img></td>
    <td>sc_type_edge_common | sc_type_var</td>
    <td>ScType::EdgeUCommonVar</td>
    <td>_<=></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_common_orient.png"></img></td>
    <td>sc_type_arc_common | sc_type_const</td>
    <td>ScType::EdgeDCommonConst</td>
    <td>=> <br> <=</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_common_orient.png"></img></td>
    <td>sc_type_arc_common | sc_type_var</td>
    <td>ScType::EdgeDCommonVar</td>
    <td>_<= <br> <=_ <br> _=></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_access.png"></img></td>
    <td>sc_type_arc_access</td>
    <td>ScType::EdgeAccess</td>
    <td>sc_arc_access <br> sc_edge_access <br> ..> <br> <.. </td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_pos_perm.png"></img></td>
    <td>sc_type_arc_access | sc_type_const | sc_type_arc_pos | sc_type_arc_perm</td>
    <td>ScType::EdgeAccessConstPosPerm</td>
    <td>sc_arc_main <br> sc_edge_main <br> <- <br> -></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_pos_perm.png"></img></td>
    <td>sc_type_arc_access | sc_type_var | sc_type_arc_pos | sc_type_arc_perm</td>
    <td>ScType::EdgeAccessVarPosPerm</td>
    <td>_<- <br> _-> <br> <-_</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_neg_perm.png"></img></td>
    <td>sc_type_arc_access | sc_type_const | sc_type_arc_neg | sc_type_arc_perm</td>
    <td>ScType::EdgeAccessConstNegPerm</td>
    <td><|- <br> -|></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_neg_perm.png"></img></td>
    <td>sc_type_arc_access | sc_type_var | sc_type_arc_neg | sc_type_arc_perm</td>
    <td>ScType::EdgeAccessVarNegPerm</td>
    <td>_<|- <br> _-|> <br> <|-_</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_fuz_perm.png"></img></td>
    <td>sc_type_arc_access | sc_type_const | sc_type_arc_fuz | sc_type_arc_perm</td>
    <td>ScType::EdgeAccessConstFuzPerm</td>
    <td><<i></i>/- <br> -/></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_fuz_perm.png"></img></td>
    <td>sc_type_arc_access | sc_type_var | sc_type_arc_fuz | sc_type_arc_perm</td>
    <td>ScType::EdgeAccessVarFuzPerm</td>
    <td>_<<i></i>/-<br>_-/><br><<i></i>/-_</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_pos_temp.png"></img></td>
    <td>sc_type_arc_access | sc_type_const | sc_type_arc_pos | sc_type_arc_temp</td>
    <td>ScType::EdgeAccessConstPosTemp</td>
    <td><~ <br> ~></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_pos_temp.png"></img></td>
    <td>sc_type_arc_access | sc_type_var | sc_type_arc_pos | sc_type_arc_temp</td>
    <td>ScType::EdgeAccessVarPosPerm</td>
    <td>_<~ <br> _~> <br> <~_</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_neg_temp.png"></img></td>
    <td>sc_type_arc_access | sc_type_const | sc_type_arc_neg | sc_type_arc_temp</td>
    <td>ScType::EdgeAccessConstNegTemp</td>
    <td><|~ <br> ~|></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_neg_temp.png"></img></td>
    <td>sc_type_arc_access | sc_type_var | sc_type_arc_neg | sc_type_arc_temp</td>
    <td>ScType::EdgeAccessVarNegPerm</td>
    <td>_<|~ <br> _~|> <br> <|~_</td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_const_fuz_temp.png"></img></td>
    <td>sc_type_arc_access | sc_type_const | sc_type_arc_fuz | sc_type_arc_temp</td>
    <td>ScType::EdgeAccessConstFuzTemp</td>
    <td><<i></i>/~ <br> ~/></td>
  </tr>

  <tr>
    <td><img src="../../images/scg/scg_edge_var_fuz_temp.png"></img></td>
    <td>sc_type_arc_access | sc_type_var | sc_type_arc_fuz | sc_type_arc_temp</td>
    <td>ScType::EdgeAccessVarFuzPerm</td>
    <td>_<<i></i>/~ <br> _~/> <br> <<i></i>/~_</td>
  </tr>

</table>
