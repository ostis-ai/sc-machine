!!! note
    This is correct for only versions of sc-machine that >= 0.9.0.
---

<table>
  <tr>
    <th>Graphical (SCg)</th>
    <th>C</th>
    <th>C++</th>
    <th>SCs</th>
    <th>10-based NS</th>
    <th>16-based NS</th>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node.png"></img></td>
    <td>sc_type_node</td>
    <td>ScType::Node</td>
    <td>sc_node</td>
    <td>1</td>
    <td>0x1</td>
  </tr>

  <tr>
    <td></td>
    <td>sc_type_const</td>
    <td>ScType::Const</td>
    <td></td>
    <td>32</td>
    <td>0x20</td>
  </tr>

  <tr>
    <td></td>
    <td>sc_type_var</td>
    <td>ScType::Var</td>
    <td></td>
    <td>64</td>
    <td>0x40</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_const.png"></img></td>
    <td>sc_type_node | sc_type_const</td>
    <td>ScType::NodeConst</td>
    <td></td>
    <td>33</td>
    <td>0x21</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_var.png"></img></td>
    <td>sc_type_node | sc_type_var</td>
    <td>ScType::NodeVar</td>
    <td></td>
    <td>65</td>
    <td>0x41</td>
  </tr>

  <tr>
    <td></td>
    <td>sc_type_node | sc_type_node_tuple</td>
    <td>ScType::NodeTuple</td>
    <td>sc_node_tuple</td>
    <td>129</td>
    <td>0x81</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_const_tuple.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_tuple</td>
    <td>ScType::NodeConstTuple</td>
    <td>sc_node_tuple <br> sc_node_not_binary_tuple</td>
    <td>161</td>
    <td>0xA1</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_var_tuple.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_tuple</td>
    <td>ScType::NodeVarTuple</td>
    <td></td>
    <td>193</td>
    <td>0xC1</td>
  </tr>

  <tr>
    <td></td>
    <td>sc_type_node | sc_type_node_structure</td>
    <td>ScType::NodeStruct</td>
    <td>sc_node_struct</td>
    <td>257</td>
    <td>0x101</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_const_struct.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_structure</td>
    <td>ScType::NodeConstStruct</td>
    <td>sc_node_struct</td>
    <td>289</td>
    <td>0x121</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_var_struct.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_structure</td>
    <td>ScType::NodeVarStruct</td>
    <td></td>
    <td>321</td>
    <td>0x141</td>
  </tr>

  <tr>
    <td></td>
    <td>sc_type_node | sc_type_node_role</td>
    <td>ScType::NodeRole</td>
    <td>sc_node_role_relation</td>
    <td>513</td>
    <td>0x201</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_const_role.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_role</td>
    <td>ScType::NodeConstRole</td>
    <td>sc_node_role_relation</td>
    <td>545</td>
    <td>0x221</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_var_role.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_role</td>
    <td>ScType::NodeVarRole</td>
    <td></td>
    <td>577</td>
    <td>0x241</td>
  </tr>

  <tr>
    <td></td>
    <td>sc_type_node | sc_type_node_norole</td>
    <td>ScType::NodeNorole</td>
    <td>sc_node_norole_relation</td>
    <td>1025</td>
    <td>0x401</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_const_norole.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_norole</td>
    <td>ScType::NodeConstNorole</td>
    <td>sc_node_norole_relation</td>
    <td>1057</td>
    <td>0x421</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_var_norole.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_norole</td>
    <td>ScType::NodeVarNorole</td>
    <td></td>
    <td>1089</td>
    <td>0x441</td>
  </tr>

  <tr>
    <td></td>
    <td>sc_type_node | sc_type_node_class</td>
    <td>ScType::NodeClass</td>
    <td>sc_node_class</td>
    <td>2049</td>
    <td>0x801</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_const_class.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_class</td>
    <td>ScType::NodeConstClass</td>
    <td>sc_node_class <br> sc_node_not_relation</td>
    <td>2081</td>
    <td>0x821</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_var_class.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_class</td>
    <td>ScType::NodeVarClass</td>
    <td></td>
    <td>2113</td>
    <td>0x841</td>
  </tr>

  <tr>
    <td></td>
    <td>sc_type_node | sc_type_node_material</td>
    <td>ScType::NodeMaterial</td>
    <td>sc_node_material</td>
    <td>8191</td>
    <td>0x2001</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_const_material.png"></img></td>
    <td>sc_type_node | sc_type_const | sc_type_node_material</td>
    <td>ScType::NodeConstMaterial</td>
    <td>sc_node_material</td>
    <td>8225</td>
    <td>0x2021</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_node_var_material.png"></img></td>
    <td>sc_type_node | sc_type_var | sc_type_node_material</td>
    <td>ScType::NodeVarMaterial</td>
    <td></td>
    <td>8257</td>
    <td>0x2041</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_link_const.png"></img></td>
    <td>sc_type_node_link_const</td>
    <td>ScType::Link</td>
    <td>sc_link</td>
    <td>34</td>
    <td>0x22</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_link_var.png"></img></td>
    <td>sc_type_node_link_var</td>
    <td>ScType::LinkVar</td>
    <td>sc_link_var</td>
    <td>66</td>
    <td>0x42</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_common.png"></img></td>
    <td>sc_type_common_edge</td>
    <td>ScType::EdgeUCommon</td>
    <td>sc_edge<br>sc_edge_ucommon<br><></td>
    <td>4</td>
    <td>0x04</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_common_orient.png"></img></td>
    <td>sc_type_common_arc</td>
    <td>ScType::EdgeDCommon</td>
    <td>sc_arc_common <br> sc_edge_common <br> < <br> ></td>
    <td>8</td>
    <td>0x08</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_const_common.png"></img></td>
    <td>sc_type_common_edge | sc_type_const</td>
    <td>ScType::EdgeUCommonConst</td>
    <td><=></td>
    <td>36</td>
    <td>0x24</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_var_common.png"></img></td>
    <td>sc_type_common_edge | sc_type_var</td>
    <td>ScType::EdgeUCommonVar</td>
    <td>_<=></td>
    <td>68</td>
    <td>0x44</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_const_common_orient.png"></img></td>
    <td>sc_type_common_arc | sc_type_const</td>
    <td>ScType::EdgeDCommonConst</td>
    <td>=> <br> <=</td>
    <td>40</td>
    <td>0x28</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_var_common_orient.png"></img></td>
    <td>sc_type_common_arc | sc_type_var</td>
    <td>ScType::EdgeDCommonVar</td>
    <td>_<= <br> <=_ <br> _=></td>
    <td>72</td>
    <td>0x48</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_access.png"></img></td>
    <td>sc_type_membership_arc</td>
    <td>ScType::EdgeAccess</td>
    <td>sc_arc_access <br> sc_edge_access <br> ..> <br> <.. </td>
    <td>16</td>
    <td>0x10</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_const_pos_perm.png"></img></td>
    <td>sc_type_membership_arc | sc_type_const | sc_type_pos_arc | sc_type_perm_arc</td>
    <td>ScType::EdgeAccessConstPosPerm</td>
    <td>sc_arc_main <br> sc_edge_main <br> <- <br> -></td>
    <td>2224</td>
    <td>0x8B0</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_var_pos_perm.png"></img></td>
    <td>sc_type_membership_arc | sc_type_var | sc_type_pos_arc | sc_type_perm_arc</td>
    <td>ScType::EdgeAccessVarPosPerm</td>
    <td>_<- <br> _-> <br> <-_</td>
    <td>2256</td>
    <td>0x8D0</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_const_neg_perm.png"></img></td>
    <td>sc_type_membership_arc | sc_type_const | sc_type_neg_arc | sc_type_perm_arc</td>
    <td>ScType::EdgeAccessConstNegPerm</td>
    <td><|- <br> -|></td>
    <td>2352</td>
    <td>0x930</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_var_neg_perm.png"></img></td>
    <td>sc_type_membership_arc | sc_type_var | sc_type_neg_arc | sc_type_perm_arc</td>
    <td>ScType::EdgeAccessVarNegPerm</td>
    <td>_<|- <br> _-|> <br> <|-_</td>
    <td>2384</td>
    <td>0x950</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_const_fuz_perm.png"></img></td>
    <td>sc_type_membership_arc | sc_type_const | sc_type_fuz_arc | sc_type_perm_arc</td>
    <td>ScType::EdgeAccessConstFuzPerm</td>
    <td><<i></i>/- <br> -/></td>
    <td>2608</td>
    <td>0xA30</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_var_fuz_perm.png"></img></td>
    <td>sc_type_membership_arc | sc_type_var | sc_type_fuz_arc | sc_type_perm_arc</td>
    <td>ScType::EdgeAccessVarFuzPerm</td>
    <td>_<<i></i>/-<br>_-/><br><<i></i>/-_</td>
    <td>2640</td>
    <td>0xA50</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_const_pos_temp.png"></img></td>
    <td>sc_type_membership_arc | sc_type_const | sc_type_pos_arc | sc_type_temp_arc</td>
    <td>ScType::EdgeAccessConstPosTemp</td>
    <td><~ <br> ~></td>
    <td>1200</td>
    <td>0x4B0</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_var_pos_temp.png"></img></td>
    <td>sc_type_membership_arc | sc_type_var | sc_type_pos_arc | sc_type_temp_arc</td>
    <td>ScType::EdgeAccessVarPosPerm</td>
    <td>_<~ <br> _~> <br> <~_</td>
    <td>1232</td>
    <td>0x4D0</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_const_neg_temp.png"></img></td>
    <td>sc_type_membership_arc | sc_type_const | sc_type_neg_arc | sc_type_temp_arc</td>
    <td>ScType::EdgeAccessConstNegTemp</td>
    <td><|~ <br> ~|></td>
    <td>1328</td>
    <td>0x530</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_var_neg_temp.png"></img></td>
    <td>sc_type_membership_arc | sc_type_var | sc_type_neg_arc | sc_type_temp_arc</td>
    <td>ScType::EdgeAccessVarNegPerm</td>
    <td>_<|~ <br> _~|> <br> <|~_</td>
    <td>1360</td>
    <td>0x550</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_const_fuz_temp.png"></img></td>
    <td>sc_type_membership_arc | sc_type_const | sc_type_fuz_arc | sc_type_temp_arc</td>
    <td>ScType::EdgeAccessConstFuzTemp</td>
    <td><<i></i>/~ <br> ~/></td>
    <td>1584</td>
    <td>0x630</td>
  </tr>

  <tr>
    <td><img src="../images/scg/scg_edge_var_fuz_temp.png"></img></td>
    <td>sc_type_membership_arc | sc_type_var | sc_type_fuz_arc | sc_type_temp_arc</td>
    <td>ScType::EdgeAccessVarFuzPerm</td>
    <td>_<<i></i>/~ <br> _~/> <br> <<i></i>/~_</td>
    <td>1616</td>
    <td>0x650</td>
  </tr>

</table>
