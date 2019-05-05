#include "sc_type.hpp"

// sc-element types
#define sc_type_node            ScType::ValueType(0x1)
#define sc_type_link            ScType::ValueType(0x2)
#define sc_type_edge_ucommon    ScType::ValueType(0x4)
#define sc_type_edge_dcommon    ScType::ValueType(0x8)
#define sc_type_edge_access     ScType::ValueType(0x10)

// sc-element constant
#define sc_type_const           ScType::ValueType(0x20)
#define sc_type_var             ScType::ValueType(0x40)

// sc-element positivity
#define sc_type_edge_pos        ScType::ValueType(0x80)
#define sc_type_edge_neg        ScType::ValueType(0x100)
#define sc_type_edge_fuz        ScType::ValueType(0x200)

// sc-element premanently
#define sc_type_edge_temp       ScType::ValueType(0x400)
#define sc_type_edge_perm       ScType::ValueType(0x800)

// struct node types
#define sc_type_node_tuple      ScType::ValueType(0x80)
#define sc_type_node_struct     ScType::ValueType(0x100)
#define sc_type_node_role       ScType::ValueType(0x200)
#define sc_type_node_norole     ScType::ValueType(0x400)
#define sc_type_node_class      ScType::ValueType(0x800)
#define sc_type_node_abstract   ScType::ValueType(0x1000)
#define sc_type_node_material   ScType::ValueType(0x2000)

// type mask
#define sc_type_element_mask     (sc_type_node | sc_type_link | sc_type_edge_ucommon | sc_type_edge_dcommon | sc_type_edge_access)
#define sc_type_constancy_mask   (sc_type_const | sc_type_var)
#define sc_type_positivity_mask  (sc_type_edge_pos | sc_type_edge_neg | sc_type_edge_fuz)
#define sc_type_permanency_mask  (sc_type_edge_perm | sc_type_edge_temp)
#define sc_type_node_mask        (sc_type_node_tuple | sc_type_node_struct | sc_type_node_role | sc_type_node_norole | sc_type_node_class | sc_type_node_abstract | sc_type_node_material)
#define sc_type_arc_mask         (sc_type_edge_access | sc_type_edge_dcommon | sc_type_edge_ucommon)

ScType const ScType::EdgeUCommon(sc_type_edge_ucommon);
ScType const ScType::EdgeDCommon(sc_type_edge_dcommon);

ScType const ScType::EdgeUCommonConst(sc_type_edge_ucommon | sc_type_const);
ScType const ScType::EdgeDCommonConst(sc_type_edge_dcommon | sc_type_const);

ScType const ScType::EdgeAccess(sc_type_edge_access);
ScType const ScType::EdgeAccessConstPosPerm(sc_type_const | sc_type_edge_access | sc_type_edge_perm | sc_type_edge_pos);
ScType const ScType::EdgeAccessConstNegPerm(sc_type_const | sc_type_edge_access | sc_type_edge_perm | sc_type_edge_neg);
ScType const ScType::EdgeAccessConstFuzPerm(sc_type_const | sc_type_edge_access | sc_type_edge_perm | sc_type_edge_fuz);
ScType const ScType::EdgeAccessConstPosTemp(sc_type_const | sc_type_edge_access | sc_type_edge_temp | sc_type_edge_pos);
ScType const ScType::EdgeAccessConstNegTemp(sc_type_const | sc_type_edge_access | sc_type_edge_temp | sc_type_edge_neg);
ScType const ScType::EdgeAccessConstFuzTemp(sc_type_const | sc_type_edge_access | sc_type_edge_temp | sc_type_edge_fuz);


ScType const ScType::EdgeUCommonVar(sc_type_edge_ucommon | sc_type_var);
ScType const ScType::EdgeDCommonVar(sc_type_edge_dcommon | sc_type_var);
ScType const ScType::EdgeAccessVarPosPerm(sc_type_var | sc_type_edge_access | sc_type_edge_perm | sc_type_edge_pos);
ScType const ScType::EdgeAccessVarNegPerm(sc_type_var | sc_type_edge_access | sc_type_edge_perm | sc_type_edge_neg);
ScType const ScType::EdgeAccessVarFuzPerm(sc_type_var | sc_type_edge_access | sc_type_edge_perm | sc_type_edge_fuz);
ScType const ScType::EdgeAccessVarPosTemp(sc_type_var | sc_type_edge_access | sc_type_edge_temp | sc_type_edge_pos);
ScType const ScType::EdgeAccessVarNegTemp(sc_type_var | sc_type_edge_access | sc_type_edge_temp | sc_type_edge_neg);
ScType const ScType::EdgeAccessVarFuzTemp(sc_type_var | sc_type_edge_access | sc_type_edge_temp | sc_type_edge_fuz);

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

ScType::ScType(ValueType type)
  : m_value(type)
{
}

bool ScType::IsEdge() const
{
  return (m_value & sc_type_arc_mask) != 0;
}

bool ScType::IsNode() const
{
  return (m_value & sc_type_node) != 0;
}

bool ScType::IsLink() const
{
  return (m_value & sc_type_link) != 0;
}

bool ScType::IsConst() const
{
  return (m_value & sc_type_const) != 0;
}

bool ScType::IsVar() const
{
  return (m_value & sc_type_var) != 0;
}

bool ScType::IsUnknown() const
{
  return (m_value == 0);
}

bool ScType::HasConstancyFlag() const
{
  return (m_value & sc_type_constancy_mask) != 0;
}

ScType ScType::AsConst() const
{
  return ScType((m_value & ~sc_type_var) | sc_type_const);
}

// Returns copy of this type, but replace constancy type upward (metavar -> var -> const)
ScType ScType::UpConstType() const
{
  /// TODO: metavar
  return ScType((m_value & ~sc_type_var) | sc_type_const); // copied from asConst for maximum perfomance
}

bool ScType::operator == (ScType const & other) const
{
  return (m_value == other.m_value);
}

bool ScType::operator != (ScType const & other) const
{
  return (m_value != other.m_value);
}

ScType ScType::operator | (ScType const & other) const
{
  return ScType(m_value | other.m_value);
}

ScType ScType::operator & (ScType const & other) const
{
  return ScType(m_value & other.m_value);
}

ScType & ScType::operator |= (ScType const & other)
{
  m_value |= other.m_value;
  return *this;
}

ScType & ScType::operator &= (ScType const & other)
{
  m_value &= other.m_value;
  return *this;
}

bool ScType::CanExtendTo(ScType const & extType) const
{
  ValueType const selfSemType = m_value & sc_type_element_mask;
  ValueType const extSemType = extType.m_value & sc_type_element_mask;

  // check semantic type
  if (selfSemType != 0 && selfSemType != extSemType)
    return false;

  // check constancy
  ValueType const selfConstType = m_value & sc_type_constancy_mask;
  ValueType const extConstType = extType.m_value & sc_type_constancy_mask;

  if (selfConstType != 0 && selfConstType != extConstType)
    return false;

  auto const CheckMask = [&extType](ValueType const & mask)
  {
    return !(extType.m_value != (extType.m_value & mask));
  };

  if (IsLink())
  {
    if (!CheckMask(sc_type_constancy_mask | sc_type_link))
      return false;
  }
  else if (IsNode())
  {
    if (!CheckMask(sc_type_constancy_mask | sc_type_node_mask | sc_type_node))
      return false;

    ValueType const selfNodeType = m_value & sc_type_node_mask;
    ValueType const extNodeType = extType.m_value & sc_type_node_mask;
    if (selfNodeType != 0 && selfNodeType != extNodeType)
      return false;
  }
  else if (IsEdge())
  {
    if (!CheckMask(sc_type_edge_dcommon | sc_type_constancy_mask))
      return false;

    if (!CheckMask(sc_type_edge_access | sc_type_constancy_mask | sc_type_positivity_mask | sc_type_permanency_mask))
      return false;

    if (!CheckMask(sc_type_edge_ucommon))
      return false;

    ValueType const selfPermType = m_value & sc_type_permanency_mask;
    ValueType const extPermType = extType.m_value & sc_type_permanency_mask;
    if (selfPermType != 0 && selfPermType != extPermType)
      return false;

    ValueType const selfPosType = m_value & sc_type_positivity_mask;
    ValueType const extPosType = extType.m_value & sc_type_positivity_mask;
    if (selfPosType != 0 && selfPosType != extPosType)
      return false;
  }

  return true;
}
