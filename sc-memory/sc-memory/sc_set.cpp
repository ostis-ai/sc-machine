/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_structure.hpp"
#include "sc_memory.hpp"
#include "sc_template.hpp"

#include "sc_keynodes.hpp"

// Hash function for ScType pairs
struct ScSet::ScTypeHashFunc
{
  std::size_t operator()(std::pair<ScType, ScType> const & p) const
  {
    std::size_t hash1 = p.first;
    std::size_t hash2 = p.second;

    return hash1 ^ hash2 ^ (hash1 << 1) ^ (hash2 << 1);
  }
};

// Equality function for ScType pairs
struct _SC_EXTERN ScSet::ScTypeEqualFunc
{
  bool operator()(std::pair<ScType, ScType> const & a, std::pair<ScType, ScType> const & b) const
  {
    return (a.first.BitAnd(b.first) == a.first) && (a.second.BitAnd(b.second) == a.second);
  }
};

std::unordered_map<std::pair<ScType, ScType>, std::array<ScType, 3>, ScSet::ScTypeHashFunc, ScSet::ScTypeEqualFunc>
    ScSet::m_setsAccessArcTypesToUnionAndInteractionResultAccessArcTypes = {
        {{ScType::EdgeAccessConstPos, ScType::EdgeAccessConstPos},
         {ScType::EdgeAccessConstPos, ScType::EdgeAccessConstPos, ScType::EdgeAccessConstNeg}},
        {{ScType::EdgeAccessConstPos, ScType::EdgeAccessConstFuz},
         {ScType::EdgeAccessConstPos, ScType::EdgeAccessConstFuz, ScType::EdgeAccessConstFuz}},
        {{ScType::EdgeAccessConstFuz, ScType::EdgeAccessConstPos},
         {ScType::EdgeAccessConstPos, ScType::EdgeAccessConstFuz, ScType::EdgeAccessConstNeg}},
        {{ScType::EdgeAccessConstPos, ScType::EdgeAccessConstNeg},
         {ScType::EdgeAccessConstPos, ScType::EdgeAccessConstNeg, ScType::EdgeAccessConstPos}},
        {{ScType::EdgeAccessConstNeg, ScType::EdgeAccessConstPos},
         {ScType::EdgeAccessConstPos, ScType::EdgeAccessConstNeg, ScType::EdgeAccessConstNeg}},

        {{ScType::EdgeAccessConstFuz, ScType::EdgeAccessConstFuz},
         {ScType::EdgeAccessConstFuz, ScType::EdgeAccessConstFuz, ScType::EdgeAccessConstFuz}},
        {{ScType::EdgeAccessConstFuz, ScType::EdgeAccessConstNeg},
         {ScType::EdgeAccessConstFuz, ScType::EdgeAccessConstNeg, ScType::EdgeAccessConstFuz}},
        {{ScType::EdgeAccessConstNeg, ScType::EdgeAccessConstFuz},
         {ScType::EdgeAccessConstFuz, ScType::EdgeAccessConstNeg, ScType::EdgeAccessConstNeg}},

        {{ScType::EdgeAccessConstNeg, ScType::EdgeAccessConstNeg},
         {ScType::EdgeAccessConstNeg, ScType::EdgeAccessConstNeg, ScType::EdgeAccessConstNeg}},
};

size_t const ScSet::UNITE_SETS_ID = 0;
size_t const ScSet::INTERSECT_SETS_ID = 1;
size_t const ScSet::SUBTRACT_SETS_ID = 2;

ScSet::ScSet(ScMemoryContext * context, ScAddr const & setAddr)
  : ScAddr(setAddr)
  , m_context(context)
{
}

ScSet::~ScSet() = default;

bool ScSet::Append(ScAddr const & elAddr, ScType const & arcType)
{
  if (!Has(elAddr, ScType::EdgeAccessConst))
    return m_context->GenerateConnector(arcType, *this, elAddr).IsValid();

  return false;
}

bool ScSet::Append(ScAddr const & elAddr, ScAddr const & attrAddr, ScType const & arcType)
{
  if (Has(elAddr, ScType::EdgeAccessConst))
    return false;

  ScAddr const & arcAddr = m_context->GenerateConnector(arcType, *this, elAddr);
  m_context->GenerateConnector(ScType::EdgeAccessConstPosPerm, attrAddr, arcAddr);

  return true;
}

bool ScSet::Remove(ScAddr const & elAddr, ScType const & arcType)
{
  bool isFound = false;
  ScIterator3Ptr const iterator3 = m_context->CreateIterator3(*this, arcType, elAddr);
  while (iterator3->Next())
  {
    m_context->EraseElement(iterator3->Get(1));
    isFound = true;
  }

  return isFound;
}

bool ScSet::Has(ScAddr const & elAddr, ScType const & arcType) const
{
  return m_context->CheckConnector(*this, elAddr, arcType);
}

ScSet & ScSet::operator<<(ScAddr const & elAddr)
{
  Append(elAddr);
  return *this;
}

ScSet & ScSet::operator<<(ScTemplateResultItem const & searchResultItem)
{
  size_t const size = searchResultItem.Size();
  for (size_t i = 0; i < size; ++i)
    Append(searchResultItem.m_replacementConstruction[i]);

  return *this;
}

ScSet & ScSet::operator>>(ScAddr const & elAddr)
{
  Remove(elAddr);
  return *this;
}

bool ScSet::IsEmpty() const
{
  ScIterator3Ptr const iterator3 = m_context->CreateIterator3(*this, ScType::EdgeAccessPos, ScType::Unknown);
  return !iterator3->Next();
}

size_t ScSet::GetPower() const
{
  size_t power = 0;
  ScIterator3Ptr const iterator3 = m_context->Iterator3(*this, ScType::EdgeAccessPos, ScType::Unknown);
  while (iterator3->Next())
    power++;
  return power;
}

ScSet & ScSet::Unite(ScSet const & otherSet)
{
  *this += otherSet;
  return *this;
}

ScSet & ScSet::Intersect(ScSet const & otherSet)
{
  *this *= otherSet;
  return *this;
}

ScSet & ScSet::Subtract(ScSet const & otherSet)
{
  *this -= otherSet;
  return *this;
}

ScSet const operator+(ScSet const & leftSet, ScSet const & rightSet)
{
  ScMemoryContext * context = leftSet.m_context;

  ScAddr const & resultSetAddr =
      ScSet::SearchBinaryOperationResult(context, leftSet, rightSet, ScKeynodes::nrel_sets_union);
  if (resultSetAddr.IsValid())
    return {context, resultSetAddr};

  ScAddrToValueUnorderedMap<ScType> leftSetElements;
  bool const leftSetIsValidForOperation = leftSet.GetElements(leftSetElements);
  if (!leftSetIsValidForOperation)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to unite provided sets because left set has outgoing both permanents and temporal sc-arcs.");

  ScAddrToValueUnorderedMap<ScType> rightSetElements;
  bool const rightSetIsValidForOperation = rightSet.GetElements(rightSetElements);
  if (!rightSetIsValidForOperation)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to unite provided sets because right set has outgoing both permanents and temporal sc-arcs.");

  ScSet resultSet{context, context->GenerateNode(ScType::NodeConst)};
  ScAddrUnorderedSet addedElements;
  auto const & AppendElements = [&](ScAddrToValueUnorderedMap<ScType> const & setElements,
                                    ScAddrToValueUnorderedMap<ScType> const & otherElements)
  {
    for (auto & [elementAddr, arcType] : setElements)
    {
      if (addedElements.find(elementAddr) != addedElements.end())
        continue;

      addedElements.insert(elementAddr);

      ScType resultArcType = arcType;
      auto const & it = otherElements.find(elementAddr);
      if (it != otherElements.cend())
      {
        ScType const & otherArcType = it->second;
        resultArcType = ScSet::GetResultAccessArcType(arcType, otherArcType, ScSet::UNITE_SETS_ID)
                        | ScSet::GetPermanencyAccessArcSubtype(arcType);
      }

      resultSet.Append(elementAddr, resultArcType);
    }
  };

  AppendElements(leftSetElements, rightSetElements);
  AppendElements(rightSetElements, leftSetElements);

  ScSet::GenerateBinaryOperationResult(context, leftSet, rightSet, ScKeynodes::nrel_sets_union, resultSet);

  return resultSet;
}

ScSet const operator+=(ScSet const & leftSet, ScSet const & rightSet)
{
  return leftSet + rightSet;
}

ScSet const operator*(ScSet const & leftSet, ScSet const & rightSet)
{
  ScMemoryContext * context = leftSet.m_context;

  ScAddr const & resultSetAddr =
      ScSet::SearchBinaryOperationResult(context, leftSet, rightSet, ScKeynodes::nrel_sets_intersection);
  if (resultSetAddr.IsValid())
    return {context, resultSetAddr};

  ScAddrToValueUnorderedMap<ScType> leftSetElements;
  bool const leftSetIsValidForOperation = leftSet.GetElements(leftSetElements);
  if (!leftSetIsValidForOperation)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to intersect provided sets because left set has outgoing both permanents and temporal sc-arcs.");

  ScAddrToValueUnorderedMap<ScType> rightSetElements;
  bool const rightSetIsValidForOperation = rightSet.GetElements(rightSetElements);
  if (!rightSetIsValidForOperation)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to intersect provided sets because right set has outgoing both permanents and temporal sc-arcs.");

  // Iterate over the smaller set to reduce the number of lookups
  ScAddrToValueUnorderedMap<ScType> const & smallerSetElements =
      (leftSetElements.size() < rightSetElements.size()) ? leftSetElements : rightSetElements;
  ScAddrToValueUnorderedMap<ScType> & largerSetElements =
      (leftSetElements.size() < rightSetElements.size()) ? rightSetElements : leftSetElements;

  ScSet resultSet{context, context->GenerateNode(ScType::NodeConst)};
  for (auto & [elementAddr, arcType] : smallerSetElements)
  {
    ScType resultArcType = arcType;
    auto const & it = largerSetElements.find(elementAddr);
    if (it == largerSetElements.cend())
      resultArcType = ScType::EdgeAccessConstNegPerm;
    else
    {
      ScType const & otherArcType = it->second;
      resultArcType = ScSet::GetResultAccessArcType(arcType, otherArcType, ScSet::INTERSECT_SETS_ID)
                      | ScSet::GetPermanencyAccessArcSubtype(arcType);

      largerSetElements.erase(elementAddr);
    }

    resultSet.Append(elementAddr, resultArcType);
  }

  for (auto & [elementAddr, _] : largerSetElements)
    resultSet.Append(elementAddr, ScType::EdgeAccessConstNegPerm);

  ScSet::GenerateBinaryOperationResult(context, leftSet, rightSet, ScKeynodes::nrel_sets_intersection, resultSet);

  return resultSet;
}

ScSet const operator*=(ScSet const & leftSet, ScSet const & rightSet)
{
  return leftSet * rightSet;
}

ScSet const operator-(ScSet const & leftSet, ScSet const & rightSet)
{
  ScMemoryContext * context = leftSet.m_context;

  ScAddr const & resultSetAddr =
      ScSet::SearchBinaryOperationResult(context, leftSet, rightSet, ScKeynodes::nrel_sets_intersection);
  if (resultSetAddr.IsValid())
    return {context, resultSetAddr};

  ScAddrToValueUnorderedMap<ScType> leftSetElements;
  bool const leftSetIsValidForOperation = leftSet.GetElements(leftSetElements);
  if (!leftSetIsValidForOperation)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to subtract provided sets because left set has outgoing both permanents and temporal sc-arcs.");

  ScAddrToValueUnorderedMap<ScType> rightSetElements;
  bool const rightSetIsValidForOperation = rightSet.GetElements(rightSetElements);
  if (!rightSetIsValidForOperation)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to subtract provided sets because right set has outgoing both permanents and temporal sc-arcs.");

  // Iterate over the left set and add elements to the result set if they are not in the right set
  ScSet resultSet{context, context->GenerateNode(ScType::NodeConst)};
  for (auto & [elementAddr, arcType] : leftSetElements)
  {
    ScType resultArcType = arcType;
    auto const & it = rightSetElements.find(elementAddr);
    if (it != rightSetElements.cend())
    {
      ScType const & otherArcType = it->second;
      resultArcType = ScSet::GetResultAccessArcType(arcType, otherArcType, ScSet::SUBTRACT_SETS_ID)
                      | ScSet::GetPermanencyAccessArcSubtype(arcType);

      rightSetElements.erase(elementAddr);
    }

    resultSet.Append(elementAddr, resultArcType);
  }

  for (auto & [elementAddr, _] : rightSetElements)
    resultSet.Append(elementAddr, ScType::EdgeAccessConstNegPerm);

  ScSet::GenerateBinaryOperationResult(context, leftSet, rightSet, ScKeynodes::nrel_sets_intersection, resultSet);

  return resultSet;
}

ScSet const operator-=(ScSet const & leftSet, ScSet const & rightSet)
{
  return leftSet - rightSet;
}

bool operator==(ScSet const & leftSet, ScSet const & rightSet)
{
  ScMemoryContext * context = leftSet.m_context;

  ScTemplate resultTempl;
  resultTempl.Quintuple(
      leftSet,
      ScType::EdgeUCommonVar,
      rightSet,
      ScType::EdgeAccessVar >> "_access_arc",
      ScKeynodes::nrel_sets_equality);
  ScTemplateSearchResult searchResult;
  if (context->HelperSearchTemplate(resultTempl, searchResult))
    return context->GetElementType(searchResult[0]["_access_arc"]).BitAnd(ScType::EdgeAccessConstPos)
           == ScType::EdgeAccessConstPos;

  ScAddrToValueUnorderedMap<ScType> leftSetElements;
  bool const leftSetIsValidForOperation = leftSet.GetElements(leftSetElements);
  if (!leftSetIsValidForOperation)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to compare provided sets because left set has outgoing both permanents and temporal sc-arcs.");

  ScAddrToValueUnorderedMap<ScType> rightSetElements;
  bool const rightSetIsValidForOperation = rightSet.GetElements(rightSetElements);
  if (!rightSetIsValidForOperation)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to compare provided sets because right set has outgoing both permanents and temporal sc-arcs.");

  bool isEqual = false;
  if (leftSetElements.size() == rightSetElements.size())
  {
    bool result = true;
    for (auto & [elementAddr, arcType] : leftSetElements)
    {
      auto const & it = rightSetElements.find(elementAddr);
      if (it == rightSetElements.cend() || it->second != arcType)
      {
        result = false;
        break;
      }
    }

    if (result)
      isEqual = true;
  }

  resultTempl.Clear();
  resultTempl.Quintuple(
      leftSet,
      ScType::EdgeUCommonVar,
      rightSet,
      isEqual ? ScType::EdgeAccessVarPosPerm : ScType::EdgeAccessVarNegPerm,
      ScKeynodes::nrel_sets_equality);
  ScTemplateGenResult genResult;
  context->HelperGenTemplate(resultTempl, genResult);

  return isEqual;
}

void ScSet::GetBinaryOperationResultTemplate(
    ScAddr const & leftSet,
    ScAddr const & rightSet,
    ScAddr const & relationAddr,
    ScAddr const & resultSetAddr,
    ScTemplate & resultTemplate)
{
  ScType connectorType = ScType::EdgeUCommonVar;
  if (relationAddr == ScKeynodes::nrel_sets_difference)
    connectorType = ScType::EdgeDCommonVar;

  resultTemplate.Clear();
  resultTemplate.Triple(leftSet, connectorType >> "_pair", rightSet);
  resultTemplate.Quintuple(
      "_pair",
      ScType::EdgeDCommonVar,
      resultSetAddr.IsValid() ? (resultSetAddr >> "_operation_result") : (ScType::NodeVar >> "_operation_result"),
      ScType::EdgeAccessVarPosPerm,
      relationAddr);
}

ScAddr ScSet::SearchBinaryOperationResult(
    ScMemoryContext * context,
    ScAddr const & leftSet,
    ScAddr const & rightSet,
    ScAddr const & relationAddr,
    ScAddr const & resultSetAddr)
{
  ScTemplate resultTemplate;
  GetBinaryOperationResultTemplate(leftSet, rightSet, relationAddr, resultSetAddr, resultTemplate);

  ScTemplateSearchResult searchResult;
  if (context->HelperSearchTemplate(resultTemplate, searchResult))
    return searchResult[0]["_operation_result"];

  return ScAddr::Empty;
}

ScAddr ScSet::GenerateBinaryOperationResult(
    ScMemoryContext * context,
    ScAddr const & leftSet,
    ScAddr const & rightSet,
    ScAddr const & relationAddr,
    ScAddr const & resultSetAddr)
{
  ScTemplate resultTemplate;
  GetBinaryOperationResultTemplate(leftSet, rightSet, relationAddr, resultSetAddr, resultTemplate);

  ScTemplateGenResult genResult;
  context->HelperGenTemplate(resultTemplate, genResult);
  return genResult["_operation_result"];
}

bool ScSet::GetElements(ScAddrToValueUnorderedMap<ScType> & elements) const
{
  bool setHasPermArc = false;
  bool setHasTempArc = false;
  ScIterator3Ptr const iterator3 = m_context->Iterator3(*this, ScType::EdgeAccessConst, ScType::Unknown);
  while (iterator3->Next())
  {
    ScAddr const & elementAddr = iterator3->Get(2);
    ScType const & arcType = m_context->GetElementType(iterator3->Get(1));

    if (arcType.BitAnd(ScType::EdgeAccessConstPerm) == ScType::EdgeAccessConstPerm)
      setHasPermArc = true;

    if (arcType.BitAnd(ScType::EdgeAccessConstTemp) == ScType::EdgeAccessConstTemp)
      setHasTempArc = true;

    elements.insert({elementAddr, arcType});
  }

  return !(setHasPermArc && setHasTempArc);
}

ScType ScSet::GetResultAccessArcType(ScType const & arcType, ScType const & otherArcType, size_t operationId)
{
  auto const & resultArcTypeIt =
      ScSet::m_setsAccessArcTypesToUnionAndInteractionResultAccessArcTypes.find({arcType, otherArcType});
  if (resultArcTypeIt != ScSet::m_setsAccessArcTypesToUnionAndInteractionResultAccessArcTypes.cend())
    return resultArcTypeIt->second[operationId];
  else
    return ScType::EdgeAccessConst;
}

ScType ScSet::GetPermanencyAccessArcSubtype(ScType const & arcType)
{
  if (arcType.BitAnd(ScType::EdgeAccessConstPerm) == ScType::EdgeAccessConstPerm)
    return ScType::EdgeAccessConstPerm;
  else if (arcType.BitAnd(ScType::EdgeAccessConstTemp) == ScType::EdgeAccessConstTemp)
    return ScType::EdgeAccessConstTemp;

  return ScType::EdgeAccessConst;
>>>>>>> 23d5bcb2 ([memory][set] Implement union, intersection, subtraction and equality of sets)
}
