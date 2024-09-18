!!! note
    This is correct for only versions of sc-machine that >= 0.10.0.
---

| С++ name                     | Value | Hex Value |
|------------------------------|-------|-----------|
| ScType::Unknown              | 0     | 0x0       |
| ScType::Node                 | 1     | 0x1       |
| ScType::Connector            | 16384 | 0x4000    |
| ScType::CommonEdge           | 16388 | 0x4004    |
| ScType::Arc                  | 49152 | 0xC000    |
| ScType::CommonArc            | 49160 | 0xC008    |
| ScType::MembershipArc        | 49168 | 0xC010    |
| ScType::Const                | 32    | 0x20      |
| ScType::Var                  | 64    | 0x40      |
| ScType::ConstNode            | 33    | 0x21      |
| ScType::VarNode              | 65    | 0x41      |
| ScType::ConstConnector       | 16416 | 0x4020    |
| ScType::VarConnector         | 16448 | 0x4040    |
| ScType::ConstCommonEdge      | 16420 | 0x4024    |
| ScType::VarCommonEdge        | 16452 | 0x4044    |
| ScType::ConstArc             | 49184 | 0xC020    |
| ScType::VarArc               | 49216 | 0xC040    |
| ScType::ConstCommonArc       | 49192 | 0xC028    |
| ScType::VarCommonArc         | 49224 | 0xC048    |
| ScType::ConstMembershipArc   | 49200 | 0xC030    |
| ScType::VarMembershipArc     | 49232 | 0xC050    |
| ScType::PermArc              | 55312 | 0xD810    |
| ScType::TempArc              | 50192 | 0xC410    |
| ScType::ConstPermArc         | 55344 | 0xD830    |
| ScType::VarPermArc           | 55376 | 0xD850    |
| ScType::ConstTempArc         | 50224 | 0xC430    |
| ScType::VarTempArc           | 50256 | 0xC450    |
| ScType::ActualTempArc        | 54288 | 0xD410    |
| ScType::InactualTempArc      | 58384 | 0xE410    |
| ScType::ConstActualTempArc   | 54320 | 0xD430    |
| ScType::VarActualTempArc     | 54352 | 0xD450    |
| ScType::ConstInactualTempArc | 58416 | 0xE430    |
| ScType::VarInactualTempArc   | 58448 | 0xE450    |
| ScType::PosArc               | 49296 | 0xC090    |
| ScType::NegArc               | 49424 | 0xC110    |
| ScType::FuzArc               | 49680 | 0xC210    |
| ScType::ConstPosArc          | 49328 | 0xC0B0    |
| ScType::VarPosArc            | 49360 | 0xC0D0    |
| ScType::PermPosArc           | 55440 | 0xD890    |
| ScType::TempPosArc           | 50320 | 0xC490    |
| ScType::ActualTempPosArc     | 54416 | 0xD490    |
| ScType::InactualTempPosArc   | 58512 | 0xE490    |
| ScType::ConstPermPosArc      | 55472 | 0xD8B0    |
| ScType::ConstTempPosArc      | 50352 | 0xC4B0    |
| ScType::VarPermPosArc        | 55504 | 0xD8D0    |
| ScType::VarTempPosArc        | 50384 | 0xC4D0    |
| ScType::ConstNegArc          | 49456 | 0xC130    |
| ScType::VarNegArc            | 49488 | 0xC150    |
| ScType::PermNegArc           | 55568 | 0xD910    |
| ScType::TempNegArc           | 50448 | 0xC510    |
| ScType::ActualTempNegArc     | 54544 | 0xD510    |
| ScType::InactualTempNegArc   | 58640 | 0xE510    |
| ScType::ConstPermNegArc      | 55600 | 0xD930    |
| ScType::ConstTempNegArc      | 50480 | 0xC530    |
| ScType::VarPermNegArc        | 55632 | 0xD950    |
| ScType::VarTempNegArc        | 50512 | 0xC550    |
| ScType::ConstFuzArc          | 49712 | 0xC230    |
| ScType::VarFuzArc            | 49744 | 0xC250    |
| ScType::NodeLink             | 3     | 0x3       |
| ScType::NodeLinkClass        | 2051  | 0x803     |
| ScType::NodeTuple            | 129   | 0x81      |
| ScType::NodeStructure        | 257   | 0x101     |
| ScType::NodeRole             | 513   | 0x201     |
| ScType::NodeNoRole           | 1025  | 0x401     |
| ScType::NodeClass            | 2049  | 0x801     |
| ScType::NodeSuperclass       | 4097  | 0x1001    |
| ScType::NodeMaterial         | 8193  | 0x2001    |
| ScType::ConstNodeLink        | 35    | 0x23      |
| ScType::ConstNodeLinkClass   | 2083  | 0x823     |
| ScType::ConstNodeTuple       | 161   | 0xA1      |
| ScType::ConstNodeStructure   | 289   | 0x121     |
| ScType::ConstNodeRole        | 545   | 0x221     |
| ScType::ConstNodeNoRole      | 1057  | 0x421     |
| ScType::ConstNodeClass       | 2081  | 0x821     |
| ScType::ConstNodeSuperclass  | 4129  | 0x1021    |
| ScType::ConstNodeMaterial    | 8225  | 0x2021    |
| ScType::VarNodeLink          | 67    | 0x43      |
| ScType::VarNodeLinkClass     | 2115  | 0x843     |
| ScType::VarNodeTuple         | 193   | 0xC1      |
| ScType::VarNodeStructure     | 321   | 0x141     |
| ScType::VarNodeRole          | 577   | 0x241     |
| ScType::VarNodeNoRole        | 1089  | 0x441     |
| ScType::VarNodeClass         | 2113  | 0x841     |
| ScType::VarNodeSuperclass    | 4161  | 0x1041    |
| ScType::VarNodeMaterial      | 8257  | 0x2041    |

<table>
  <tr>
    <th>C++ name</th>
    <th>C name</th>
    <th>Graphical (SCg)</th>
    <th>SCs</th>
  </tr>

  <tr>
    <td>ScType::Node</td>
    <td>sc_type_node</td>
    <td><img src="../images/scg/scg_node.png"></img></td>
    <td>sc_node</td>
  </tr>

  <tr>
    <td>ScType::Const</td>
    <td>sc_type_const</td>
    <td></td>
    <td></td>
  </tr>

  <tr>
    <td>ScType::Var</td>
    <td>sc_type_var</td>
    <td></td>
    <td></td>
  </tr>

  <tr>
    <td>ScType::ConstNode</td>
    <td>sc_type_const | sc_type_node</td>
    <td><img src="../images/scg/scg_node_const.png"></img></td>
    <td></td>
  </tr>

  <tr>
    <td>ScType::VarNode</td>
    <td>sc_type_var | sc_type_node</td>
    <td><img src="../images/scg/scg_node_var.png"></img></td>
    <td></td>
  </tr>

  <tr>
    <td>ScType::CommonEdge</td>
    <td>sc_type_common_edge</td>
    <td><img src="../images/scg/scg_edge_common.png"></img></td>
    <td>sc_edge<br><></td>
  </tr>

  <tr>
    <td>ScType::CommonArc</td>
    <td>sc_type_common_arc</td>
    <td><img src="../images/scg/scg_edge_common_orient.png"></img></td>
    <td>sc_arc_common <br> < <br> ></td>
  </tr>

  <tr>
    <td>ScType::ConstCommonEdge</td>
    <td>sc_type_const | sc_type_common_edge</td>
    <td><img src="../images/scg/scg_edge_const_common.png"></img></td>
    <td><=></td>
  </tr>

  <tr>
    <td>ScType::VarCommonEdge</td>
    <td>sc_type_var | sc_type_common_edge</td>
    <td><img src="../images/scg/scg_edge_var_common.png"></img></td>
    <td>_<=></td>
  </tr>

  <tr>
    <td>ScType::ConstCommonArc</td>
    <td>sc_type_const | sc_type_common_arc</td>
    <td><img src="../images/scg/scg_edge_const_common_orient.png"></img></td>
    <td>=> <br> <=</td>
  </tr>

  <tr>
    <td>ScType::VarCommonArc</td>
    <td>sc_type_var | sc_type_common_arc</td>
    <td><img src="../images/scg/scg_edge_var_common_orient.png"></img></td>
    <td>_<= <br> <=_ <br> _=></td>
  </tr>

  <tr>
    <td>ScType::MembershipArc</td>
    <td>sc_type_membership_arc</td>
    <td><img src="../images/scg/scg_edge_access.png"></img></td>
    <td>sc_arc_access <br> ..> <br> <.. </td>
  </tr>

  <tr>
    <td>ScType::ConstPermPosArc</td>
    <td>sc_type_const | sc_type_perm_arc | sc_type_pos_arc</td>
    <td><img src="../images/scg/scg_edge_const_pos_perm.png"></img></td>
    <td>sc_arc_main <br> <- <br> -></td>
  </tr>

  <tr>
    <td>ScType::VarPermPosArc</td>
    <td>sc_type_var | sc_type_perm_arc | sc_type_pos_arc</td>
    <td><img src="../images/scg/scg_edge_var_pos_perm.png"></img></td>
    <td>_<- <br> _-> <br> <-_</td>
  </tr>

  <tr>
    <td>ScType::ConstPermNegArc</td>
    <td>sc_type_const | sc_type_perm_arc | sc_type_neg_arc</td>
    <td><img src="../images/scg/scg_edge_const_neg_perm.png"></img></td>
    <td><|- <br> -|></td>
  </tr>

  <tr>
    <td>ScType::VarPermNegArc</td>
    <td>sc_type_var | sc_type_perm_arc | sc_type_neg_arc</td>
    <td><img src="../images/scg/scg_edge_var_neg_perm.png"></img></td>
    <td>_<|- <br> _-|> <br> <|-_</td>
  </tr>

  <tr>
    <td>ScType::ConstFuzArc</td>
    <td>sc_type_const | sc_type_fuz_arc</td>
    <td><img src="../images/scg/scg_edge_const_fuz_perm.png"></img></td>
    <td><<i></i>/- <br> -/></td>
  </tr>

  <tr>
    <td>ScType::VarFuzArc</td>
    <td>sc_type_var | sc_type_fuz_arc</td>
    <td><img src="../images/scg/scg_edge_var_fuz_perm.png"></img></td>
    <td>_<<i></i>/-<br>_-/><br><<i></i>/-_</td>
  </tr>

  <tr>
    <td>ScType::ConstTempPosArc</td>
    <td>sc_type_const | sc_type_temp_arc | sc_type_pos_arc</td>
    <td><img src="../images/scg/scg_edge_const_pos_temp.png"></img></td>
    <td><~ <br> ~></td>
  </tr>

  <tr>
    <td>ScType::VarTempPosArc</td>
    <td>sc_type_var | sc_type_temp_arc | sc_type_pos_arc</td>
    <td><img src="../images/scg/scg_edge_var_pos_temp.png"></img></td>
    <td>_<~ <br> _~> <br> <~_</td>
  </tr>

  <tr>
    <td>ScType::ConstTempNegArc</td>
    <td>sc_type_const | sc_type_temp_arc | sc_type_neg_arc</td>
    <td><img src="../images/scg/scg_edge_const_neg_temp.png"></img></td>
    <td><|~ <br> ~|></td>
  </tr>

  <tr>
    <td>ScType::VarTempNegArc</td>
    <td>sc_type_var | sc_type_temp_arc | sc_type_neg_arc</td>
    <td><img src="../images/scg/scg_edge_var_neg_temp.png"></img></td>
    <td>_<|~ <br> _~|> <br> <|~_</td>
  </tr>

  <tr>
    <td>ScType::NodeLink</td>
    <td>sc_type_node_link</td>
    <td></td>
    <td></td>
  </tr>

  <tr>
    <td>ScType::ConstNodeLink</td>
    <td>sc_type_const | sc_type_node_link</td>
    <td><img src="../images/scg/scg_link_const.png"></img></td>
    <td>sc_link</td>
  </tr>

  <tr>
    <td>ScType::VarNodeLink</td>
    <td>sc_type_var | sc_type_node_link</td>
    <td><img src="../images/scg/scg_link_var.png"></img></td>
    <td>sc_link_var</td>
  </tr>

  <tr>
    <td>ScType::NodeTuple</td>
    <td>sc_type_node_tuple</td>
    <td></td>
    <td>sc_node_tuple</td>
  </tr>

  <tr>
    <td>ScType::ConstNodeTuple</td>
    <td>sc_type_const | sc_type_node_tuple</td>
    <td><img src="../images/scg/scg_node_const_tuple.png"></img></td>
    <td>sc_node_tuple</td>
  </tr>

  <tr>
    <td>ScType::VarNodeTuple</td>
    <td>sc_type_var | sc_type_node_tuple</td>
    <td><img src="../images/scg/scg_node_var_tuple.png"></img></td>
    <td></td>
  </tr>

  <tr>
    <td>ScType::NodeStructure</td>
    <td>sc_type_node_structure</td>
    <td></td>
    <td>sc_node_struct</td>
  </tr>

  <tr>
    <td>ScType::ConstNodeStructure</td>
    <td>sc_type_const | sc_type_node_structure</td>
    <td><img src="../images/scg/scg_node_const_struct.png"></img></td>
    <td>sc_node_struct</td>
  </tr>

  <tr>
    <td>ScType::VarNodeStructure</td>
    <td>sc_type_var | sc_type_node_structure</td>
    <td><img src="../images/scg/scg_node_var_struct.png"></img></td>
    <td></td>
  </tr>

  <tr>
    <td>ScType::NodeRole</td>
    <td>sc_type_node_role</td>
    <td></td>
    <td></td>
  </tr>

  <tr>
    <td>ScType::ConstNodeRole</td>
    <td>sc_type_const | sc_type_node_role</td>
    <td><img src="../images/scg/scg_node_const_role.png"></img></td>
    <td>sc_node_role_relation</td>
  </tr>

  <tr>
    <td>ScType::VarNodeRole</td>
    <td>sc_type_var | sc_type_node_role</td>
    <td><img src="../images/scg/scg_node_var_role.png"></img></td>
    <td></td>
  </tr>

  <tr>
    <td>ScType::NodeNorole</td>
    <td>sc_type_node_norole</td>
    <td></td>
    <td>sc_node_norole_relation</td>
  </tr>

  <tr>
    <td>ScType::NodeConstNorole</td>
    <td>sc_type_const | sc_type_node_norole</td>
    <td><img src="../images/scg/scg_node_const_norole.png"></img></td>
    <td>sc_node_norole_relation</td>
  </tr>

  <tr>
    <td>ScType::NodeVarNorole</td>
    <td>sc_type_var | sc_type_node_norole</td>
    <td><img src="../images/scg/scg_node_var_norole.png"></img></td>
    <td></td>
  </tr>

  <tr>
    <td>ScType::NodeClass</td>
    <td>sc_type_node_class</td>
    <td></td>
    <td></td>
  </tr>

  <tr>
    <td>ScType::ConstNodeClass</td>
    <td>sc_type_const | sc_type_node_class</td>
    <td><img src="../images/scg/scg_node_const_class.png"></img></td>
    <td>sc_node_class</td>
  </tr>

  <tr>
    <td>ScType::VarNodeClass</td>
    <td>sc_type_var | sc_type_node_class</td>
    <td><img src="../images/scg/scg_node_var_class.png"></img></td>
    <td></td>
  </tr>

  <tr>
    <td>ScType::NodeMaterial</td>
    <td>sc_type_node_material</td>
    <td></td>
    <td>sc_node_material</td>
  </tr>

  <tr>
    <td>ScType::ConstNodeMaterial</td>
    <td>sc_type_const | sc_type_node_material</td>
    <td><img src="../images/scg/scg_node_const_material.png"></img></td>
    <td>sc_node_material</td>
  </tr>

  <tr>
    <td>ScType::VarNodeMaterial</td>
    <td>sc_type_var | sc_type_node_material</td>
    <td><img src="../images/scg/scg_node_var_material.png"></img></td>
    <td></td>
  </tr>
</table>
