#include <gtest/gtest.h>

#include <bitset>
#include <iomanip>

#include "sc-memory/sc_memory.hpp"

#include "sc_test.hpp"

#define Y true
#define N false

std::vector<std::pair<ScType, std::tuple<bool, bool, bool, bool, bool, bool, bool, bool, bool>>>
GetTypesToSubtypesVector()
{
  /* clang-format off */
  return {                        // IsNode     IsLink  IsConnector IsCommonEdge  IsArc IsCommonArc IsMembershipArc IsConst   IsVar
      {ScType::Unknown,                {N,         N,         N,         N,         N,         N,         N,         N,         N}},

      {ScType::Node,                   {Y,         N,         N,         N,         N,         N,         N,         N,         N}},
      {ScType::Connector,              {N,         N,         Y,         N,         N,         N,         N,         N,         N}},
      {ScType::CommonEdge,             {N,         N,         Y,         Y,         N,         N,         N,         N,         N}},
      {ScType::Arc,                    {N,         N,         Y,         N,         Y,         N,         N,         N,         N}},
      {ScType::CommonArc,              {N,         N,         Y,         N,         Y,         Y,         N,         N,         N}},
      {ScType::MembershipArc,          {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},

      // Constancy types
      {ScType::Const,                  {N,         N,         N,         N,         N,         N,         N,         Y,         N}},
      {ScType::Var,                    {N,         N,         N,         N,         N,         N,         N,         N,         Y}},

      {ScType::ConstNode,              {Y,         N,         N,         N,         N,         N,         N,         Y,         N}},
      {ScType::VarNode,                {Y,         N,         N,         N,         N,         N,         N,         N,         Y}},
      {ScType::ConstConnector,         {N,         N,         Y,         N,         N,         N,         N,         Y,         N}},
      {ScType::VarConnector,           {N,         N,         Y,         N,         N,         N,         N,         N,         Y}},
      {ScType::ConstCommonEdge,        {N,         N,         Y,         Y,         N,         N,         N,         Y,         N}},
      {ScType::VarCommonEdge,          {N,         N,         Y,         Y,         N,         N,         N,         N,         Y}},
      {ScType::ConstArc,               {N,         N,         Y,         N,         Y,         N,         N,         Y,         N}},
      {ScType::VarArc,                 {N,         N,         Y,         N,         Y,         N,         N,         N,         Y}},
      {ScType::ConstCommonArc,         {N,         N,         Y,         N,         Y,         Y,         N,         Y,         N}},
      {ScType::VarCommonArc,           {N,         N,         Y,         N,         Y,         Y,         N,         N,         Y}},
      {ScType::ConstMembershipArc,     {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::VarMembershipArc,       {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},

      // Permanency types
      {ScType::PermArc,                {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},
      {ScType::TempArc,                {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},

      {ScType::ConstPermArc,           {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::VarPermArc,             {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},
      {ScType::ConstTempArc,           {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::VarTempArc,             {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},

      // Actuality types
      {ScType::ActualTempArc,          {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},
      {ScType::InactualTempArc,        {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},

      {ScType::ConstActualTempArc,     {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::VarActualTempArc,       {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},
      {ScType::ConstInactualTempArc,   {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::VarInactualTempArc,     {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},

      // Positivity types
      {ScType::PosArc,                 {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},
      {ScType::NegArc,                 {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},
      {ScType::FuzArc,                 {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},

      // Positive sc-arcs
      {ScType::ConstPosArc,            {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::VarPosArc,              {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},

      {ScType::PermPosArc,             {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},
      {ScType::TempPosArc,             {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},
      {ScType::ActualTempPosArc,       {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},
      {ScType::InactualTempPosArc,     {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},

      {ScType::ConstPermPosArc,        {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::ConstTempPosArc,        {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::ConstActualTempPosArc,  {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::ConstInactualTempPosArc,{N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},

      {ScType::VarPermPosArc,          {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},
      {ScType::VarTempPosArc,          {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},
      {ScType::VarActualTempPosArc,    {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},
      {ScType::VarInactualTempPosArc,  {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},

      // Negative sc-arcs
      {ScType::ConstNegArc,            {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::VarNegArc,              {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},

      {ScType::PermNegArc,             {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},
      {ScType::TempNegArc,             {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},
      {ScType::ActualTempNegArc,       {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},
      {ScType::InactualTempNegArc,     {N,         N,         Y,         N,         Y,         N,         Y,         N,         N}},

      {ScType::ConstPermNegArc,        {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::ConstTempNegArc,        {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::ConstActualTempNegArc,  {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::ConstInactualTempNegArc,{N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},

      {ScType::VarPermNegArc,          {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},
      {ScType::VarTempNegArc,          {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},
      {ScType::VarActualTempNegArc,    {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},
      {ScType::VarInactualTempNegArc,  {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},

      // Fuzzy sc-arcs
      {ScType::ConstFuzArc,            {N,         N,         Y,         N,         Y,         N,         Y,         Y,         N}},
      {ScType::VarFuzArc,              {N,         N,         Y,         N,         Y,         N,         Y,         N,         Y}},

      // Semantic sc-node types
      {ScType::NodeLink,               {Y,         Y,         N,         N,         N,         N,         N,         N,         N}},
      {ScType::NodeLinkClass,          {Y,         Y,         N,         N,         N,         N,         N,         N,         N}},
      {ScType::NodeTuple,              {Y,         N,         N,         N,         N,         N,         N,         N,         N}},
      {ScType::NodeStructure,          {Y,         N,         N,         N,         N,         N,         N,         N,         N}},
      {ScType::NodeRole,               {Y,         N,         N,         N,         N,         N,         N,         N,         N}},
      {ScType::NodeNoRole,             {Y,         N,         N,         N,         N,         N,         N,         N,         N}},
      {ScType::NodeClass,              {Y,         N,         N,         N,         N,         N,         N,         N,         N}},
      {ScType::NodeSuperclass,         {Y,         N,         N,         N,         N,         N,         N,         N,         N}},
      {ScType::NodeMaterial,           {Y,         N,         N,         N,         N,         N,         N,         N,         N}},

      // Constant semantic node types
      {ScType::ConstNodeLink,          {Y,         Y,         N,         N,         N,         N,         N,         Y,         N}},
      {ScType::ConstNodeLinkClass,     {Y,         Y,         N,         N,         N,         N,         N,         Y,         N}},
      {ScType::ConstNodeTuple,         {Y,         N,         N,         N,         N,         N,         N,         Y,         N}},
      {ScType::ConstNodeStructure,     {Y,         N,         N,         N,         N,         N,         N,         Y,         N}},
      {ScType::ConstNodeRole,          {Y,         N,         N,         N,         N,         N,         N,         Y,         N}},
      {ScType::ConstNodeNoRole,        {Y,         N,         N,         N,         N,         N,         N,         Y,         N}},
      {ScType::ConstNodeClass,         {Y,         N,         N,         N,         N,         N,         N,         Y,         N}},
      {ScType::ConstNodeSuperclass,    {Y,         N,         N,         N,         N,         N,         N,         Y,         N}},
      {ScType::ConstNodeMaterial,      {Y,         N,         N,         N,         N,         N,         N,         Y,         N}},

      // Variable semantic node types
      {ScType::VarNodeLink,            {Y,         Y,         N,         N,         N,         N,         N,         N,         Y}},
      {ScType::VarNodeLinkClass,       {Y,         Y,         N,         N,         N,         N,         N,         N,         Y}},
      {ScType::VarNodeTuple,           {Y,         N,         N,         N,         N,         N,         N,         N,         Y}},
      {ScType::VarNodeStructure,       {Y,         N,         N,         N,         N,         N,         N,         N,         Y}},
      {ScType::VarNodeRole,            {Y,         N,         N,         N,         N,         N,         N,         N,         Y}},
      {ScType::VarNodeNoRole,          {Y,         N,         N,         N,         N,         N,         N,         N,         Y}},
      {ScType::VarNodeClass,           {Y,         N,         N,         N,         N,         N,         N,         N,         Y}},
      {ScType::VarNodeSuperclass,      {Y,         N,         N,         N,         N,         N,         N,         N,         Y}},
      {ScType::VarNodeMaterial,        {Y,         N,         N,         N,         N,         N,         N,         N,         Y}},
  };
  /* clang-format on */
}

struct ScType::ScTypeHashFunc
{
  sc_type operator()(ScType const & type) const
  {
    return type;
  }
};

class TestScType : public ScType
{
public:
  constexpr TestScType(sc_type type)
    : ScType(type)
  {
  }

  static auto const & GetTypesToNames()
  {
    return m_typesToNames;
  }

  TestScType BitNand(ScType const & other) const
  {
    return this->BitAnd(~other);
  }
};

TEST(ScTypeTest, CheckTypeSubtypes)
{
  auto const & typeVector = GetTypesToSubtypesVector();
  for (auto const & [type, subtypes] : typeVector)
  {
    auto const [isNode, isLink, isConnector, isCommonEdge, isArc, isCommonArc, isMembershipArc, isConst, isVar] =
        subtypes;
    EXPECT_EQ(type.IsNode(), isNode);
    EXPECT_EQ(type.IsLink(), isLink);
    EXPECT_EQ(type.IsConnector(), isConnector);
    EXPECT_EQ(type.IsCommonEdge(), isCommonEdge);
    EXPECT_EQ(type.IsArc(), isArc);
    EXPECT_EQ(type.IsCommonArc(), isCommonArc);
    EXPECT_EQ(type.IsMembershipArc(), isMembershipArc);
    EXPECT_EQ(type.IsConst(), isConst);
    EXPECT_EQ(type.IsVar(), isVar);
  }

  EXPECT_EQ(TestScType::GetTypesToNames().size(), typeVector.size());
}

std::vector<std::pair<ScType, ScType>> GetDeprecatedTypesToTypesMap()
{
  SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_BEGIN
  return {
      {ScType::EdgeUCommon, ScType::CommonEdge},
      {ScType::EdgeUCommonConst, ScType::ConstCommonEdge},
      {ScType::EdgeUCommonVar, ScType::VarCommonEdge},
      {ScType::EdgeDCommon, ScType::CommonArc},
      {ScType::EdgeDCommonConst, ScType::ConstCommonArc},
      {ScType::EdgeDCommonVar, ScType::VarCommonArc},
      {ScType::EdgeAccessConstPosPerm, ScType::ConstPermPosArc},
      {ScType::EdgeAccessConstNegPerm, ScType::ConstPermNegArc},
      {ScType::EdgeAccessConstFuzPerm, ScType::ConstFuzArc},
      {ScType::EdgeAccessConstPosTemp, ScType::ConstTempPosArc},
      {ScType::EdgeAccessConstNegTemp, ScType::ConstTempNegArc},
      {ScType::EdgeAccessConstFuzTemp, ScType::ConstFuzArc},
      {ScType::EdgeAccessVarPosPerm, ScType::VarPermPosArc},
      {ScType::EdgeAccessVarNegPerm, ScType::VarPermNegArc},
      {ScType::EdgeAccessVarFuzPerm, ScType::VarFuzArc},
      {ScType::EdgeAccessVarPosTemp, ScType::VarTempPosArc},
      {ScType::EdgeAccessVarNegTemp, ScType::VarTempNegArc},
      {ScType::EdgeAccessVarFuzTemp, ScType::VarFuzArc},
      {ScType::NodeConst, ScType::ConstNode},
      {ScType::NodeVar, ScType::VarNode},
      {ScType::Link, ScType::NodeLink},
      {ScType::LinkClass, ScType::NodeLinkClass},
      {ScType::NodeStruct, ScType::NodeStructure},
      {ScType::LinkConst, ScType::ConstNodeLink},
      {ScType::LinkConstClass, ScType::ConstNodeLinkClass},
      {ScType::NodeConstStruct, ScType::ConstNodeStructure},
      {ScType::NodeConstTuple, ScType::ConstNodeTuple},
      {ScType::NodeConstRole, ScType::ConstNodeRole},
      {ScType::NodeConstNoRole, ScType::ConstNodeNoRole},
      {ScType::NodeConstClass, ScType::ConstNodeClass},
      {ScType::NodeConstMaterial, ScType::ConstNodeMaterial},
      {ScType::LinkVar, ScType::VarNodeLink},
      {ScType::LinkVarClass, ScType::VarNodeLinkClass},
      {ScType::NodeVarStruct, ScType::VarNodeStructure},
      {ScType::NodeVarTuple, ScType::VarNodeTuple},
      {ScType::NodeVarRole, ScType::VarNodeRole},
      {ScType::NodeVarNoRole, ScType::VarNodeNoRole},
      {ScType::NodeVarClass, ScType::VarNodeClass},
      {ScType::NodeVarMaterial, ScType::VarNodeMaterial},
  };
  SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_END
}

TEST(ScTypeTest, CheckDeprecatedTypes)
{
  auto const & typeVector = GetDeprecatedTypesToTypesMap();
  for (auto const & [deprecatedType, type] : typeVector)
    EXPECT_EQ(deprecatedType, type);
}

TEST(ScTypeTest, ExtendTypes)
{
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::Unknown));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::Node));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::ConstNode));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::NodeLink));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::CommonEdge));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::CommonArc));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::MembershipArc));
  EXPECT_TRUE(ScType::Unknown.CanExtendTo(ScType::ConstFuzArc));

  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::Node));
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::ConstNode));
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::VarNode));
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::NodeMaterial));
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::ConstNodeMaterial));
  EXPECT_TRUE(ScType::Node.CanExtendTo(ScType::NodeLink));
  EXPECT_FALSE(ScType::Node.CanExtendTo(ScType::Connector));
  EXPECT_FALSE(ScType::Node.CanExtendTo(ScType::Arc));
  EXPECT_FALSE(ScType::Node.CanExtendTo(ScType::CommonEdge));
  EXPECT_FALSE(ScType::Node.CanExtendTo(ScType::CommonArc));
  EXPECT_FALSE(ScType::Node.CanExtendTo(ScType::MembershipArc));

  EXPECT_TRUE(ScType::NodeLink.CanExtendTo(ScType::NodeLink));
  EXPECT_TRUE(ScType::NodeLink.CanExtendTo(ScType::ConstNodeLink));
  EXPECT_TRUE(ScType::NodeLink.CanExtendTo(ScType::VarNodeLink));
  EXPECT_TRUE(ScType::NodeLink.CanExtendTo(ScType::NodeLinkClass));
  EXPECT_FALSE(ScType::NodeLink.CanExtendTo(ScType::Node));

  EXPECT_FALSE(ScType::Connector.CanExtendTo(ScType::Unknown));
  EXPECT_FALSE(ScType::Connector.CanExtendTo(ScType::Node));
  EXPECT_TRUE(ScType::Connector.CanExtendTo(ScType::Connector));
  EXPECT_TRUE(ScType::Connector.CanExtendTo(ScType::Arc));
  EXPECT_TRUE(ScType::Connector.CanExtendTo(ScType::CommonEdge));
  EXPECT_TRUE(ScType::Connector.CanExtendTo(ScType::CommonArc));
  EXPECT_TRUE(ScType::Connector.CanExtendTo(ScType::MembershipArc));

  EXPECT_FALSE(ScType::CommonEdge.CanExtendTo(ScType::Unknown));
  EXPECT_FALSE(ScType::CommonEdge.CanExtendTo(ScType::Node));
  EXPECT_FALSE(ScType::CommonEdge.CanExtendTo(ScType::Connector));
  EXPECT_FALSE(ScType::CommonEdge.CanExtendTo(ScType::Arc));
  EXPECT_TRUE(ScType::CommonEdge.CanExtendTo(ScType::CommonEdge));
  EXPECT_TRUE(ScType::CommonEdge.CanExtendTo(ScType::ConstCommonEdge));
  EXPECT_FALSE(ScType::CommonEdge.CanExtendTo(ScType::CommonArc));
  EXPECT_FALSE(ScType::CommonEdge.CanExtendTo(ScType::MembershipArc));

  EXPECT_FALSE(ScType::Arc.CanExtendTo(ScType::Unknown));
  EXPECT_FALSE(ScType::Arc.CanExtendTo(ScType::Node));
  EXPECT_FALSE(ScType::Arc.CanExtendTo(ScType::Connector));
  EXPECT_TRUE(ScType::Arc.CanExtendTo(ScType::Arc));
  EXPECT_TRUE(ScType::Arc.CanExtendTo(ScType::CommonArc));
  EXPECT_TRUE(ScType::Arc.CanExtendTo(ScType::MembershipArc));
  EXPECT_FALSE(ScType::Arc.CanExtendTo(ScType::CommonEdge));

  EXPECT_FALSE(ScType::CommonArc.CanExtendTo(ScType::Unknown));
  EXPECT_FALSE(ScType::CommonArc.CanExtendTo(ScType::Node));
  EXPECT_FALSE(ScType::CommonArc.CanExtendTo(ScType::Arc));
  EXPECT_FALSE(ScType::CommonArc.CanExtendTo(ScType::Connector));
  EXPECT_TRUE(ScType::CommonArc.CanExtendTo(ScType::CommonArc));
  EXPECT_TRUE(ScType::CommonArc.CanExtendTo(ScType::ConstCommonArc));
  EXPECT_FALSE(ScType::CommonArc.CanExtendTo(ScType::CommonEdge));
  EXPECT_FALSE(ScType::CommonArc.CanExtendTo(ScType::MembershipArc));

  EXPECT_FALSE(ScType::MembershipArc.CanExtendTo(ScType::Unknown));
  EXPECT_FALSE(ScType::MembershipArc.CanExtendTo(ScType::Node));
  EXPECT_FALSE(ScType::MembershipArc.CanExtendTo(ScType::Arc));
  EXPECT_FALSE(ScType::MembershipArc.CanExtendTo(ScType::Connector));
  EXPECT_TRUE(ScType::MembershipArc.CanExtendTo(ScType::MembershipArc));
  EXPECT_TRUE(ScType::MembershipArc.CanExtendTo(ScType::ConstPermPosArc));
  EXPECT_FALSE(ScType::MembershipArc.CanExtendTo(ScType::CommonArc));
  EXPECT_FALSE(ScType::MembershipArc.CanExtendTo(ScType::CommonEdge));
  EXPECT_FALSE(ScType::MembershipArc.CanExtendTo(ScType::NodeLink));

  EXPECT_FALSE(ScType::Const.CanExtendTo(ScType::Var));
  EXPECT_TRUE(ScType::ConstFuzArc.CanExtendTo(ScType::ConstFuzArc));
  EXPECT_FALSE(ScType::ConstFuzArc.CanExtendTo(ScType::ConstPermNegArc));
  EXPECT_FALSE(ScType::ConstFuzArc.CanExtendTo(ScType::VarFuzArc));
}

TEST(ScTypeTest, CheckReverseSCsConnectors)
{
  auto const & typeVector = GetTypesToSubtypesVector();

  for (auto const & [type, _] : typeVector)
  {
    if (type.IsCommonArc() || type.IsMembershipArc())
    {
      std::string reverseSCsConnector = type.GetReverseSCsConnector();
      EXPECT_NE(reverseSCsConnector.find('<'), std::string::npos);
      std::reverse(reverseSCsConnector.begin(), reverseSCsConnector.end());
      std::replace(reverseSCsConnector.begin(), reverseSCsConnector.end(), '<', '>');
      EXPECT_EQ(type.GetDirectSCsConnector(), reverseSCsConnector);
    }
  }
}

std::string PrintTabulatedString(size_t n, std::string const & string)
{
  std::cout << std::left << std::setw(n) << string;
  return "";
}

std::string PrintHyphens(size_t n)
{
  while (n > 0)
  {
    std::cout << "-";
    --n;
  }

  return "";
}

void PrintTableCap(
    std::string const & firstColumnName,
    size_t firstColumnLength,
    std::string const & secondColumnName,
    size_t secondColumnLength,
    std::string const & thirdColumnName,
    size_t thirdColumnLength)
{
  std::cout << "| " << PrintTabulatedString(firstColumnLength, firstColumnName) << " | "
            << PrintTabulatedString(secondColumnLength, secondColumnName) << " | "
            << PrintTabulatedString(thirdColumnLength, thirdColumnName) << " |" << std::endl;
  std::cout << "|" << PrintHyphens(firstColumnLength + 2) << "|" << PrintHyphens(secondColumnLength + 2) << "|"
            << PrintHyphens(thirdColumnLength + 2) << "|" << std::endl;
}

TEST(ScTypeTest, PrintTypesToBitValues)
{
  auto const & typeVector = GetTypesToSubtypesVector();

  std::size_t const firstColumnLength = 8u + 23u;
  std::size_t const secondColumnLength = 13u;
  std::size_t const thirdColumnLength = 9u;

  PrintTableCap("C++ name", firstColumnLength, "Decimal value", secondColumnLength, "Hex value", thirdColumnLength);

  for (auto const & [type, _] : typeVector)
  {
    std::cout << "| " << PrintTabulatedString(firstColumnLength, "ScType::" + std::string(type)) << " | "
              << PrintTabulatedString(secondColumnLength, std::to_string(type)) << " | "
              << "0x" << std::left << std::setw(thirdColumnLength - 2) << std::hex << std::uppercase << type << std::dec
              << " |\n";
  }
}

std::string CamelToSnake(std::string const & name)
{
  std::string snakeCase;
  for (size_t i = 0; i < name.length(); ++i)
  {
    sc_char currentChar = name[i];

    if (std::isupper(currentChar) && i != 0)
      snakeCase += '_';

    snakeCase += std::tolower(currentChar);
  }

  return snakeCase;
}

std::string ConvertToSCgFile(ScType const & type)
{
  return type.GetDirectSCsConnector().find_first_of('?') == std::string::npos
             ? "<img src=\"../images/scg/scg_" + CamelToSnake(type) + ".png\"></img>"
             : "Not specified";
}

TEST(ScTypeTest, PrintTypesToSCsSCgConnectors)
{
  auto const & typeVector = GetTypesToSubtypesVector();

  std::size_t const firstColumnLength = 8u + 23u;
  std::size_t const secondColumnLength = 70u;
  std::size_t const thirdColumnLength = 20u;

  PrintTableCap("C++ name", firstColumnLength, "SCg-code", secondColumnLength, "SCs-code", thirdColumnLength);

  for (auto const & [type, _] : typeVector)
  {
    if (type.IsCommonEdge() || type.IsCommonArc() || type.IsMembershipArc())
    {
      std::string const & directSCsConnector = type.GetDirectSCsConnector();

      std::cout << "| " << PrintTabulatedString(firstColumnLength, "ScType::" + std::string(type)) << " | "
                << PrintTabulatedString(secondColumnLength, ConvertToSCgFile(type)) << " | ";

      if (type.IsCommonEdge())
        std::cout << PrintTabulatedString(thirdColumnLength, "```" + directSCsConnector + "```") << " |\n";
      else
        std::cout << PrintTabulatedString(
            thirdColumnLength, "```" + directSCsConnector + " or " + type.GetReverseSCsConnector() + "```")
                  << " |\n";
    }
  }
}

TEST(ScTypeTest, PrintTypesToSCgSCsNodes)
{
  auto const & typeVector = GetTypesToSubtypesVector();

  std::size_t const firstColumnLength = 8u + 23u;
  std::size_t const secondColumnLength = 70u;
  std::size_t const thirdColumnLength = 25u;

  PrintTableCap("C++ name", firstColumnLength, "SCg-code", secondColumnLength, "SCs-code", thirdColumnLength);

  for (auto const & [type, _] : typeVector)
  {
    if (type.IsNode())
    {
      std::string const & nodeKeynode =
          TestScType(type).BitNand(ScType::Const).BitNand(ScType::Var).GetSCsElementKeynode();

      std::cout << "| " << PrintTabulatedString(firstColumnLength, "ScType::" + std::string(type)) << " | " << std::left
                << PrintTabulatedString(secondColumnLength, ConvertToSCgFile(type)) << " | ";
      std::cout << PrintTabulatedString(thirdColumnLength, nodeKeynode) << " |\n";
    }
  }
}

TEST(ScTypeTest, PrintSCsToSCgConnectors)
{
  auto const & typeVector = GetTypesToSubtypesVector();

  std::size_t const firstColumnLength = 20u;
  std::size_t const secondColumnLength = 70u;

  std::cout << "| " << PrintTabulatedString(firstColumnLength, "SCs-code") << " | "
            << PrintTabulatedString(secondColumnLength, "SCg-code") << " |\n";
  std::cout << "| " << PrintHyphens(firstColumnLength) << " | " << PrintHyphens(secondColumnLength) << " |\n";

  for (auto const & [type, _] : typeVector)
  {
    if (type.IsCommonEdge() || type.IsCommonArc() || type.IsMembershipArc())
    {
      std::string const & directSCsConnector = type.GetDirectSCsConnector();

      if (type.IsCommonEdge())
        std::cout << "| " << PrintTabulatedString(firstColumnLength, "```" + directSCsConnector + "```");
      else
        std::cout << "| "
                  << PrintTabulatedString(
                         firstColumnLength,
                         "```" + directSCsConnector + " or " + type.GetReverseSCsConnector() + "```");

      std::cout << " | " << PrintTabulatedString(secondColumnLength, ConvertToSCgFile(type)) << " |\n";
    }
  }
}
