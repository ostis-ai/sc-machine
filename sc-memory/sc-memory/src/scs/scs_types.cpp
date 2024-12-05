/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */
#include "sc-memory/scs/scs_types.hpp"

#include "sc-memory/sc_debug.hpp"

namespace scs
{

TypeResolver::SCsDesignationsToScTypes TypeResolver::ms_connectorsToTypes = {
    {"?<=>", ScType::CommonEdge},

    {"?=>", ScType::CommonArc},
    {"<=?", ScType::CommonArc},

    {"?.?>", ScType::MembershipArc}, 
    {"<?.?", ScType::MembershipArc},

    {"<=>", ScType::ConstCommonEdge},
    {"_<=>", ScType::VarCommonEdge},

    {"=>", ScType::ConstCommonArc},
    {"<=", ScType::ConstCommonArc},
    {"_=>", ScType::VarCommonArc},
    {"<=_", ScType::VarCommonArc},

    {".?>", ScType::ConstMembershipArc}, 
    {"<?.", ScType::ConstMembershipArc},
    {"_.?>", ScType::VarMembershipArc}, 
    {"<?._", ScType::VarMembershipArc},

    {"?.>", ScType::PosArc},
    {"<.?", ScType::PosArc},

    {"?.|>", ScType::NegArc},
    {"<|.?", ScType::NegArc},

    {"?/>", ScType::FuzArc},
    {"</?", ScType::FuzArc},

    {".>", ScType::ConstPosArc},
    {"<.", ScType::ConstPosArc},
    {"_.>", ScType::VarPosArc},
    {"<._", ScType::VarPosArc},      

    {".|>", ScType::ConstNegArc},
    {"<|.", ScType::ConstNegArc},
    {"_.|>", ScType::VarNegArc},
    {"<|._", ScType::VarNegArc},

    {"?-?>", ScType::PermArc},
    {"<?-?", ScType::PermArc},

    {"?..?>", ScType::TempArc},
    {"<?..?", ScType::TempArc},

    {"?~?>", ScType::ActualTempArc},
    {"<?~?", ScType::ActualTempArc},

    {"?%?>", ScType::InactualTempArc},
    {"<?%?", ScType::InactualTempArc},

    {"-?>", ScType::ConstPermArc},
    {"<?-", ScType::ConstPermArc},
    {"_-?>", ScType::VarPermArc},
    {"<?-_", ScType::VarPermArc},

    {"..?>", ScType::ConstTempArc},
    {"<?..", ScType::ConstTempArc},
    {"_..?>", ScType::VarTempArc},
    {"<?.._", ScType::VarTempArc},

    {"~?>", ScType::ConstActualTempArc},
    {"<?~", ScType::ConstActualTempArc},
    {"_~?>", ScType::VarActualTempArc},
    {"<?~_", ScType::VarActualTempArc},

    {"%?>", ScType::ConstInactualTempArc},
    {"<?%", ScType::ConstInactualTempArc},
    {"_%?>", ScType::VarInactualTempArc},
    {"<?%_", ScType::VarInactualTempArc},

    {"?->", ScType::PermPosArc},
    {"<-?", ScType::PermPosArc},

    {"?-|>", ScType::PermNegArc},
    {"<|-?", ScType::PermNegArc},

    {"?..>", ScType::TempPosArc},
    {"<..?", ScType::TempPosArc},

    {"?~>", ScType::ActualTempPosArc},
    {"<~?", ScType::ActualTempPosArc},

    {"?%>", ScType::InactualTempPosArc},
    {"<%?", ScType::InactualTempPosArc},

    {"?..|>", ScType::TempNegArc},
    {"<|..?", ScType::TempNegArc},

    {"?~|>", ScType::ActualTempNegArc},
    {"<|~?", ScType::ActualTempNegArc},

    {"?%|>", ScType::InactualTempNegArc},
    {"<|%?", ScType::InactualTempNegArc},

    {"->", ScType::ConstPermPosArc},
    {"<-", ScType::ConstPermPosArc},
    {"_->", ScType::VarPermPosArc},
    {"<-_", ScType::VarPermPosArc},

    {"-|>", ScType::ConstPermNegArc},
    {"<|-", ScType::ConstPermNegArc},
    {"_-|>", ScType::VarPermNegArc},
    {"<|-_", ScType::VarPermNegArc},

    {"/>", ScType::ConstFuzArc},
    {"</", ScType::ConstFuzArc},

    {"_/>", ScType::VarFuzArc},
    {"</_", ScType::VarFuzArc},

    {"..>", ScType::ConstTempPosArc},
    {"<..", ScType::ConstTempPosArc},
    {"_..>", ScType::VarTempPosArc},
    {"<.._", ScType::VarTempPosArc},

    {"~>", ScType::ConstActualTempPosArc},
    {"<~", ScType::ConstActualTempPosArc},
    {"_~>", ScType::VarActualTempPosArc},
    {"<~_", ScType::VarActualTempPosArc},

    {"%>", ScType::ConstInactualTempPosArc},
    {"<%", ScType::ConstInactualTempPosArc},
    {"_%>", ScType::VarInactualTempPosArc},
    {"<%_", ScType::VarInactualTempPosArc},

    {"..|>", ScType::ConstTempNegArc},
    {"<|..", ScType::ConstTempNegArc},
    {"_..|>", ScType::VarTempNegArc},
    {"<|.._", ScType::VarTempNegArc},

    {"~|>", ScType::ConstActualTempNegArc},
    {"<|~", ScType::ConstActualTempNegArc},
    {"_~|>", ScType::VarActualTempNegArc},
    {"<|~_", ScType::VarActualTempNegArc},

    {"%|>", ScType::ConstInactualTempNegArc},
    {"<|%", ScType::ConstInactualTempNegArc},
    {"_%|>", ScType::VarInactualTempNegArc},
    {"<|%_", ScType::VarInactualTempNegArc},
};

TypeResolver::SCsDesignationsToScTypes TypeResolver::ms_deprecatedConnectorsToTypes = {
    {">", ScType::CommonArc},
    {"<>", ScType::CommonEdge},
};

TypeResolver::SCsDesignationsToScTypes TypeResolver::ms_deprecatedReverseConnectorsToTypes = {
    {"<", ScType::CommonArc},
    {"_<=", ScType::VarCommonArc},
    {"_<-", ScType::VarPermPosArc},
    {"_<|-", ScType::VarPermNegArc},
    {"_<~", ScType::VarActualTempPosArc},
    {"_<|~", ScType::VarActualTempNegArc},
};

struct TypeResolver::ScTypeHashFunc
{
  sc_type operator()(ScType const & type) const
  {
    return type;
  }
};

TypeResolver::ScTypesToSCsDesignations TypeResolver::ms_typesToConnectors = {
    {ScType::CommonEdge, "?<=>"}, 
    {ScType::CommonArc, "?=>"},
    {ScType::MembershipArc, "?.?>"},
    {ScType::ConstCommonEdge, "<=>"},
    {ScType::VarCommonEdge, "_<=>"},
    {ScType::ConstCommonArc, "=>"},
    {ScType::VarCommonArc, "_=>"},
    {ScType::ConstMembershipArc, ".?>"},
    {ScType::VarMembershipArc, "_.?>"},
    {ScType::VarArc, "_=>"},
    {ScType::PosArc, "?.>"},
    {ScType::NegArc, "?.|>"},
    {ScType::FuzArc, "?/>"},
    {ScType::PermArc, "?-?>"},
    {ScType::TempArc, "?..?>"},
    {ScType::ActualTempArc, "?~?>"},
    {ScType::InactualTempArc, "?%?>"},
    {ScType::ConstPermArc, "-?>"},
    {ScType::VarPermArc, "_-?>"},
    {ScType::ConstTempArc, "..?>"},
    {ScType::VarTempArc, "_..?>"},
    {ScType::ConstActualTempArc, "~?>"},
    {ScType::VarActualTempArc, "_~?>"},
    {ScType::ConstInactualTempArc, "%?>"},
    {ScType::VarInactualTempArc, "_%?>"},
    {ScType::PermPosArc, "?->"},
    {ScType::PermNegArc, "?-|>"},
    {ScType::TempPosArc, "?..>"},
    {ScType::ActualTempPosArc, "?~>"},
    {ScType::InactualTempPosArc, "?%>"},
    {ScType::TempNegArc, "?..|>"},
    {ScType::ActualTempNegArc, "?~|>"},
    {ScType::InactualTempNegArc, "?%|>"},
    {ScType::ConstPermPosArc, "->"},
    {ScType::VarPermPosArc, "_->"},
    {ScType::ConstPermNegArc, "-|>"},
    {ScType::VarPermNegArc, "_-|>"},
    {ScType::ConstPosArc, ".>"},
    {ScType::VarPosArc, "_.>"},
    {ScType::ConstNegArc, ".|>"},
    {ScType::VarNegArc, "_.|>"},
    {ScType::ConstFuzArc, "/>"},
    {ScType::VarFuzArc, "_/>"},
    {ScType::ConstTempPosArc, "..>"},
    {ScType::VarTempPosArc, "_..>"},
    {ScType::ConstActualTempPosArc, "~>"},
    {ScType::VarActualTempPosArc, "_~>"},
    {ScType::ConstInactualTempPosArc, "%>"},
    {ScType::VarInactualTempPosArc, "_%>"},
    {ScType::ConstTempNegArc, "..|>"},
    {ScType::VarTempNegArc, "_..|>"},
    {ScType::ConstActualTempNegArc, "~|>"},
    {ScType::VarActualTempNegArc, "_~|>"},
    {ScType::ConstInactualTempNegArc, "%|>"},
    {ScType::VarInactualTempNegArc, "_%|>"}
};

TypeResolver::ScTypesToSCsDesignations TypeResolver::ms_typesToReverseConnectors = {
    {ScType::CommonEdge, "?<=>"}, 
    {ScType::CommonArc, "<=?"},
    {ScType::MembershipArc, "<?.?"},
    {ScType::ConstCommonEdge, "<=>"},
    {ScType::VarCommonEdge, "_<=>"},
    {ScType::ConstCommonArc, "<="},
    {ScType::VarCommonArc, "<=_"},
    {ScType::ConstMembershipArc, "<?."},
    {ScType::VarMembershipArc, "<?._"},
    {ScType::VarArc, "<=_"},
    {ScType::PosArc, "<.?"},
    {ScType::NegArc, "<|.?"},
    {ScType::FuzArc, "</?"},
    {ScType::PermArc, "<?-?"},
    {ScType::TempArc, "<?..?"},
    {ScType::ActualTempArc, "<?~?"},
    {ScType::InactualTempArc, "<?%?"},
    {ScType::ConstPermArc, "<?-"},
    {ScType::VarPermArc, "<?-_"},
    {ScType::ConstTempArc, "<?.."},
    {ScType::VarTempArc, "<?.._"},
    {ScType::ConstActualTempArc, "<?~"},
    {ScType::VarActualTempArc, "<?~_"},
    {ScType::ConstInactualTempArc, "<?%"},
    {ScType::VarInactualTempArc, "<?%_"},
    {ScType::PermPosArc, "<-?"},
    {ScType::PermNegArc, "<|-?"},
    {ScType::TempPosArc, "<..?"},
    {ScType::ActualTempPosArc, "<~?"},
    {ScType::InactualTempPosArc, "<%?"},
    {ScType::TempNegArc, "<|..?"},
    {ScType::ActualTempNegArc, "<|~?"},
    {ScType::InactualTempNegArc, "<|%?"},
    {ScType::ConstPermPosArc, "<-"},
    {ScType::VarPermPosArc, "<-_"},
    {ScType::ConstPermNegArc, "<|-"},
    {ScType::VarPermNegArc, "<|-_"},
    {ScType::ConstPosArc, "<."},
    {ScType::VarPosArc, "<._"},
    {ScType::ConstNegArc, "<|."},
    {ScType::VarNegArc, "<|._"},
    {ScType::ConstFuzArc, "</"},
    {ScType::VarFuzArc, "</_"},
    {ScType::ConstTempPosArc, "<.."},
    {ScType::VarTempPosArc, "<.._"},
    {ScType::ConstActualTempPosArc, "<~"},
    {ScType::VarActualTempPosArc, "<~_"},
    {ScType::ConstInactualTempPosArc, "<%"},
    {ScType::VarInactualTempPosArc, "<%_"},
    {ScType::ConstTempNegArc, "<|.."},
    {ScType::VarTempNegArc, "<|.._"},
    {ScType::ConstActualTempNegArc, "<|~"},
    {ScType::VarActualTempNegArc, "<|~_"},
    {ScType::ConstInactualTempNegArc, "<|%"},
    {ScType::VarInactualTempNegArc, "<|%_"}
};

TypeResolver::SCsDesignationsToScTypes TypeResolver::ms_keynodesToTypes = {
    {"sc_node", ScType::Node},
    {"sc_link", ScType::NodeLink},
    {"sc_link_class", ScType::NodeLinkClass},
    {"sc_node_tuple", ScType::NodeTuple},
    {"sc_node_structure", ScType::NodeStructure},
    {"sc_node_class", ScType::NodeClass},
    {"sc_node_role_relation", ScType::NodeRole},
    {"sc_node_non_role_relation", ScType::NodeNonRole},
    {"sc_node_superclass", ScType::NodeSuperclass},
    {"sc_node_material", ScType::NodeMaterial},

    {"sc_common_edge", ScType::CommonEdge},
    {"sc_common_arc", ScType::CommonArc},
    {"sc_membership_arc", ScType::MembershipArc},
    {"sc_main_arc", ScType::ConstPermPosArc},
};

TypeResolver::SCsDesignationsToScTypes TypeResolver::ms_deprecatedKeynodesToTypes = {
    {"sc_node_not_binary_tuple", ScType::NodeTuple},
    {"sc_node_struct", ScType::NodeStructure},
    {"sc_node_not_relation", ScType::NodeClass},
    {"sc_node_norole_relation", ScType::NodeNonRole},

    {"sc_edge", ScType::CommonEdge},
    {"sc_edge_ucommon", ScType::CommonEdge},

    {"sc_arc_common", ScType::CommonArc},
    {"sc_edge_dcommon", ScType::CommonArc},

    {"sc_arc_main", ScType::ConstPermPosArc},
    {"sc_edge_main", ScType::ConstPermPosArc},

    {"sc_arc_access", ScType::MembershipArc},
    {"sc_edge_access", ScType::MembershipArc},
};

TypeResolver::ScTypesToSCsDesignations TypeResolver::ms_typesToKeynodes = {
    {ScType::CommonEdge, "sc_common_edge"},
    {ScType::CommonArc, "sc_common_arc"},
    {ScType::MembershipArc, "sc_membership_arc"},
    {ScType::ConstPermPosArc, "sc_main_arc"},

    {ScType::Node, "sc_node"},
    {ScType::NodeLink, "sc_link"},
    {ScType::NodeLinkClass, "sc_link_class"},
    {ScType::NodeTuple, "sc_node_tuple"},
    {ScType::NodeStructure, "sc_node_structure"},
    {ScType::NodeClass, "sc_node_class"},
    {ScType::NodeRole, "sc_node_role_relation"},
    {ScType::NodeNonRole, "sc_node_non_role_relation"},
    {ScType::NodeSuperclass, "sc_node_superclass"},
    {ScType::NodeMaterial, "sc_node_material"},
};

TypeResolver::SCsConnectorDesignations TypeResolver::ms_reversedConnectors = {
    "<=?", "<?.?", "<=", "<=_", 
    "<.", "<._", "<?.", "<?._", "<.?", "<|.?", "</?", "<?-?", "<?..?", "<?~?", "<?%?",
    "<-?", "<|-?", "<..?", "<~?", "<%?", "<|..?", "<|~?", "<|%?", 
    "<-", "<-_", "<|-", "<|-_", "</", "</-", "</_", "</-_", 
    "<..", "<.._", "<|..", "<|.._", "<~", "<~_", "<|~", "|<~_", "<%", "<%_", "<|%", "|<%_"
};

TypeResolver::SCsConnectorDesignations TypeResolver::ms_deprecatedReversedConnectors = {
    "<", "_<=", "_<-", "_<|-", "_<~", "_<|~",
};

std::string TypeResolver::GetDirectSCsConnector(ScType const & type)
{
  auto const it = ms_typesToConnectors.find(type);
  if (it == ms_typesToConnectors.cend())
    return "";

  return it->second;
}

std::string TypeResolver::GetReverseSCsConnector(ScType const & type)
{
  auto const it = ms_typesToReverseConnectors.find(type);
  if (it == ms_typesToReverseConnectors.cend())
    return "";

  return it->second;
}

std::string TypeResolver::GetSCsElementKeynode(ScType const & type)
{
   auto const it = ms_typesToKeynodes.find(type);
   if (it == ms_typesToKeynodes.cend())
    return "";

  return it->second;
}

ScType const & TypeResolver::GetConnectorType(std::string const & connectorAlias)
{
  auto const it = ms_connectorsToTypes.find(connectorAlias);
  if (it == ms_connectorsToTypes.cend())
  {
    auto const deprecatedIt = ms_deprecatedConnectorsToTypes.find(connectorAlias);
    if (deprecatedIt == ms_deprecatedConnectorsToTypes.cend())
    {
      auto const deprecatedReverseIt = ms_deprecatedReverseConnectorsToTypes.find(connectorAlias);
      if (deprecatedReverseIt == ms_deprecatedReverseConnectorsToTypes.cend())
        return ScType::Unknown;

      SC_LOG_WARNING("Specified reverse designation of sc-connector `" << connectorAlias << "` is deprecated"
        " in SCs-code, use `" << ms_typesToReverseConnectors[deprecatedReverseIt->second] << "` instead.");
      
      return deprecatedReverseIt->second;
    }

    SC_LOG_WARNING("Specified designation of sc-connector `" << connectorAlias << "` is deprecated"
        " in SCs-code, use `" << ms_typesToConnectors[deprecatedIt->second] << "` instead.");
    return deprecatedIt->second;
  } 
  
  return it->second;
}

ScType const & TypeResolver::GetElementType(std::string const & keynodeAlias)
{
  auto const it = ms_keynodesToTypes.find(keynodeAlias);
  if (it == ms_keynodesToTypes.cend())
  {
    auto const deprecatedIt = ms_deprecatedKeynodesToTypes.find(keynodeAlias);
    if (deprecatedIt == ms_deprecatedKeynodesToTypes.cend())
      return ScType::Unknown;

    SC_LOG_WARNING("Specified sc-type class `" << keynodeAlias << "` is deprecated in SCs-code, "
        << "use `" << ms_typesToKeynodes[deprecatedIt->second] << "` instead.");

    return deprecatedIt->second;
  }

  return it->second;
}

bool TypeResolver::IsConnectorReversed(std::string const & connectorAlias)
{
  return ms_reversedConnectors.find(connectorAlias) != ms_reversedConnectors.cend()
    || ms_deprecatedReversedConnectors.find(connectorAlias) != ms_deprecatedReversedConnectors.cend();
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

bool TypeResolver::IsElementType(std::string const & alias)
{
  return ms_keynodesToTypes.find(alias) != ms_keynodesToTypes.cend() 
     || ms_deprecatedKeynodesToTypes.find(alias) != ms_deprecatedKeynodesToTypes.cend();
}

bool TypeResolver::IsUnnamed(std::string const & alias)
{
  return alias == "..." || alias == "_...";
}

}  // namespace scs
