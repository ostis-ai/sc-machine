/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */
#include "sc-memory/scs/scs_types.hpp"

#include "sc-memory/sc_debug.hpp"

namespace scs
{

TypeResolver::MapType TypeResolver::ms_connectorToType = {
    {">", ScType::CommonArc},
    {"<", ScType::CommonArc},
    {"<>", ScType::CommonEdge},
    {"..>", ScType::MembershipArc},
    {"<..", ScType::MembershipArc},
    {"<=>", ScType::ConstCommonEdge},
    {"_<=>", ScType::VarCommonEdge},
    {"=>", ScType::ConstCommonArc},
    {"<=", ScType::ConstCommonArc},
    {"_=>", ScType::VarCommonArc},
    {"_<=", ScType::VarCommonArc},
    {"<=_", ScType::VarCommonArc},
    {"->", ScType::ConstPermPosArc},
    {"<-", ScType::ConstPermPosArc},
    {"_->", ScType::VarPermPosArc},
    {"<-_", ScType::VarPermPosArc},
    {"_<-", ScType::VarPermPosArc},
    {"-|>", ScType::ConstPermNegArc},
    {"<|-", ScType::ConstPermNegArc},
    {"_-|>", ScType::VarPermNegArc},
    {"_<|-", ScType::VarPermNegArc},
    {"<|-_", ScType::VarPermNegArc},
    {"-/>", ScType::ConstFuzArc},
    {"</-", ScType::ConstFuzArc},
    {"_-/>", ScType::VarFuzArc},
    {"_</-", ScType::VarFuzArc},
    {"</-_", ScType::VarFuzArc},
    {"~>", ScType::ConstTempPosArc},
    {"<~", ScType::ConstTempPosArc},
    {"_~>", ScType::VarTempPosArc},
    {"_<~", ScType::VarTempPosArc},
    {"<~_", ScType::VarTempPosArc},
    {"~|>", ScType::ConstTempNegArc},
    {"<|~", ScType::ConstTempNegArc},
    {"_~|>", ScType::VarTempNegArc},
    {"_<|~", ScType::VarTempNegArc},
    {"<|~_", ScType::VarTempNegArc},
    {"~/>", ScType::ConstFuzArc},
    {"</~", ScType::ConstFuzArc},
    {"_~/>", ScType::VarFuzArc},
    {"_</~", ScType::VarFuzArc},
    {"</~_", ScType::VarFuzArc}
};

// TODO: divide into const and var types
TypeResolver::MapType TypeResolver::ms_keynodeToType = {
    {"sc_node", ScType::Node},

    {"sc_link", ScType::NodeLink},
    {"sc_link_class", ScType::NodeLinkClass},

    {"sc_arc_common", ScType::CommonArc}, // backward compatibility
    {"sc_edge_dcommon", ScType::CommonArc},

    {"sc_edge", ScType::CommonEdge}, // backward compatibility
    {"sc_edge_ucommon", ScType::CommonEdge},

    {"sc_arc_main", ScType::ConstPermPosArc}, // backward compatibility
    {"sc_edge_main", ScType::ConstPermPosArc},

    {"sc_arc_access", ScType::MembershipArc}, // backward compatibility
    {"sc_edge_access", ScType::MembershipArc},

    {"sc_node_tuple", ScType::NodeTuple}, // backward compatibility
    {"sc_node_not_binary_tuple", ScType::NodeTuple},

    {"sc_node_struct", ScType::NodeStructure},
    {"sc_node_role_relation", ScType::NodeRole},
    {"sc_node_norole_relation", ScType::NodeNoRole},
    //{"sc_node_abstract", ScType::NodeAbstract}, // TODO: remove it
    {"sc_node_material", ScType::NodeMaterial},

    {"sc_node_not_relation", ScType::NodeClass}, // backward compatibility
    {"sc_node_class", ScType::NodeClass},
};

TypeResolver::IsType TypeResolver::ms_reversedConnectors =
    {"<", "<..", "<=", "_<=","<=_", "<-", "_<-","<-_", "<|-", "_<|-","<|-_", "</-", "_</-","</-_", "<~", "_<~","<~_", "<|~", "<|~_", "_<|~", "</~", "_</~", "</~_"};

ScType const & TypeResolver::GetConnectorType(std::string const & connectorAlias)
{
  auto const it = ms_connectorToType.find(connectorAlias);
  return it != ms_connectorToType.cend() ? it->second : ScType::Unknown;
}

ScType const & TypeResolver::GetKeynodeType(std::string const & keynodeAlias)
{
  auto const it = ms_keynodeToType.find(keynodeAlias);
  return it != ms_keynodeToType.cend() ? it->second : ScType::Unknown;
}

bool TypeResolver::IsConnectorReversed(std::string const & connectorAlias)
{
  return ms_reversedConnectors.find(connectorAlias) != ms_reversedConnectors.cend();
}

bool TypeResolver::IsConst(std::string const & idtf)
{
  if (idtf.empty())
    return true;

  size_t const n = idtf.size() - 1;
  size_t i = 0;
  while ((idtf[i] == '.') && (i < n))
    ++i;

  return idtf[i] != '_';
}

bool TypeResolver::IsConnectorAttrConst(std::string const & attr)
{
  return attr == ":";
}

bool TypeResolver::IsKeynodeType(std::string const & alias)
{
  return ms_keynodeToType.find(alias) != ms_keynodeToType.cend();
}

bool TypeResolver::IsUnnamed(std::string const & alias)
{
  return alias == "..." || alias == "_...";
}

}  // namespace scs
