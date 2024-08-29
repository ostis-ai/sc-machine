/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"

#include <utility>
#include <functional>
#include <algorithm>

#include "sc_template_private.hpp"
#include "sc_memory.hpp"

ScTemplateItem::ScTemplateItem()
{
  m_itemType = Type::Type;
}

ScTemplateItem::ScTemplateItem(ScAddr const & addr, sc_char const * replName)
{
  SetAddr(addr, replName);
}

ScTemplateItem::ScTemplateItem(ScType const & type, sc_char const * replName)
{
  SetType(type, replName);
}

ScTemplateItem::ScTemplateItem(sc_char const * name)
{
  SetReplacement(name);
}

ScTemplateItem::ScTemplateItem(std::string const & name)
{
  SetReplacement(name.c_str());
}

bool ScTemplateItem::IsAddr() const
{
  return m_itemType == Type::Addr;
}

bool ScTemplateItem::IsReplacement() const
{
  return m_itemType == Type::Replace;
}

bool ScTemplateItem::IsType() const
{
  return m_itemType == Type::Type;
}

bool ScTemplateItem::IsFixed() const
{
  return IsAddr() || (IsReplacement() && m_addrValue.IsValid());
}

bool ScTemplateItem::IsAssign() const
{
  return m_itemType == Type::Type;
}

bool ScTemplateItem::HasName() const
{
  return !m_name.empty();
}

[[nodiscard]] std::string ScTemplateItem::GetPrettyName() const
{
  std::string result;
  if (HasName())
  {
    result.reserve(2 + m_name.size());
    result = "`" + m_name + "`";
  }

  return result;
}

void ScTemplateItem::SetAddr(ScAddr const & addr, sc_char const * replName)
{
  m_itemType = Type::Addr;
  m_addrValue = addr;
  if (replName)
    m_name = replName;
  else if (m_name.empty())
    m_name = std::to_string(addr.Hash());
}

void ScTemplateItem::SetType(ScType const & type, sc_char const * replName)
{
  m_itemType = Type::Type;
  m_typeValue = type;
  if (replName)
    m_name = replName;
}

void ScTemplateItem::SetReplacement(sc_char const * name)
{
  m_itemType = Type::Replace;
  if (name)
    m_name = name;
}

ScTemplateItem operator>>(ScAddr const & value, sc_char const * replName)
{
  return {value, replName};
}

ScTemplateItem operator>>(ScAddr const & value, std::string const & replName)
{
  return {value, replName.c_str()};
}

ScTemplateItem operator>>(ScType const & value, sc_char const * replName)
{
  return {value, replName};
}

ScTemplateItem operator>>(ScType const & value, std::string const & replName)
{
  return {value, replName.c_str()};
}

// --------------------------------

ScTemplateTriple::ScTemplateTriple(
    ScTemplateItem const & param1,
    ScTemplateItem const & param2,
    ScTemplateItem const & param3,
    size_t index)
  : m_index(index)
{
  m_values[0] = param1;
  m_values[1] = param2;
  m_values[2] = param3;
}

ScTemplateTriple::ScTemplateTripleItems const & ScTemplateTriple::GetValues() const
{
  return m_values;
}

ScTemplateItem const & ScTemplateTriple::operator[](size_t index) const noexcept(false)
{
  if (index < m_values.size())
    return m_values[index];

  SC_THROW_EXCEPTION(
      utils::ExceptionInvalidParams,
      "Index=" + std::to_string(index) + " must be < size=" + std::to_string(m_values.size()));
}

// --------------------------------

ScTemplateParams & ScTemplateParams::Add(std::string const & varIdtf, ScAddr const & value) noexcept(false)
{
  if (m_templateItemsToParams.find(varIdtf) == m_templateItemsToParams.cend())
  {
    m_templateItemsToParams[varIdtf] = value;
    return *this;
  }

  SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Alias=" << varIdtf << " already has value");
}

ScTemplateParams & ScTemplateParams::Add(ScAddr const & varAddr, ScAddr const & value) noexcept(false)
{
  std::string const & varAddrHashStr = std::to_string(varAddr.Hash());
  if (m_templateItemsToParams.find(varAddrHashStr) == m_templateItemsToParams.cend())
  {
    m_templateItemsToParams[varAddrHashStr] = value;
    return *this;
  }

  SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Var=" << varAddrHashStr << " already has value");
}

bool ScTemplateParams::Get(std::string const & varIdtf, ScAddr & outAddr) const noexcept
{
  auto const it = m_templateItemsToParams.find(varIdtf);
  if (it != m_templateItemsToParams.cend())
  {
    outAddr = it->second;
    return true;
  }

  outAddr = ScAddr::Empty;
  return false;
}

bool ScTemplateParams::Get(ScAddr const & varAddr, ScAddr & outAddr) const noexcept
{
  std::string const & varAddrHashStr = std::to_string(varAddr.Hash());
  auto const it = m_templateItemsToParams.find(varAddrHashStr);
  if (it != m_templateItemsToParams.cend())
  {
    outAddr = it->second;
    return true;
  }

  outAddr = ScAddr::Empty;
  return false;
}

ScTemplateParams::ScTemplateItemsToParams ScTemplateParams::GetAll() const
{
  return m_templateItemsToParams;
}

bool ScTemplateParams::IsEmpty() const noexcept
{
  return m_templateItemsToParams.empty();
}

// --------------------------------

ScTemplate::Result::Result(bool result, std::string errorMsg)
  : m_result(result)
  , m_msg(std::move(errorMsg))
{
}

ScTemplate::Result::operator bool() const
{
  return m_result;
}

[[nodiscard]] std::string const & ScTemplate::Result::Msg() const
{
  return m_msg;
}

// --------------------------------

ScTemplate::ScTemplate() noexcept
{
  m_templateTriples.reserve(16);

  auto const tripleTypeCount = (size_t)ScTemplateTripleType::ScConstr3TypeCount;
  m_priorityOrderedTemplateTriples.resize(tripleTypeCount);
}

ScTemplate::~ScTemplate() noexcept
{
  for (auto * triple : m_templateTriples)
    delete triple;
  m_templateTriples.clear();
}

ScTemplate::ScTemplate(ScTemplate && other) noexcept
  : m_templateItemsNamesToReplacementItemsPositions(std::move(other.m_templateItemsNamesToReplacementItemsPositions))
  , m_templateTriples(std::move(other.m_templateTriples))
  , m_priorityOrderedTemplateTriples(std::move(other.m_priorityOrderedTemplateTriples))
  , m_templateItemsNamesToReplacementItemsAddrs(std::move(other.m_templateItemsNamesToReplacementItemsAddrs))
  , m_templateItemsNamesToTypes(std::move(other.m_templateItemsNamesToTypes))
{
}

ScTemplate & ScTemplate::operator=(ScTemplate && other) noexcept
{
  if (this == &other)
    return *this;

  m_templateItemsNamesToReplacementItemsPositions = std::move(other.m_templateItemsNamesToReplacementItemsPositions);
  m_templateTriples = std::move(other.m_templateTriples);
  m_priorityOrderedTemplateTriples = std::move(other.m_priorityOrderedTemplateTriples);
  m_templateItemsNamesToReplacementItemsAddrs = std::move(other.m_templateItemsNamesToReplacementItemsAddrs);
  m_templateItemsNamesToTypes = std::move(other.m_templateItemsNamesToTypes);

  other.Clear();
  return *this;
}

ScTemplate & ScTemplate::operator()(
    ScTemplateItem const & param1,
    ScTemplateItem const & param2,
    ScTemplateItem const & param3)
{
  return Triple(param1, param2, param3);
}

ScTemplate & ScTemplate::operator()(
    ScTemplateItem const & param1,
    ScTemplateItem const & param2,
    ScTemplateItem const & param3,
    ScTemplateItem const & param4,
    ScTemplateItem const & param5)
{
  return Quintuple(param1, param2, param3, param4, param5);
}

void ScTemplate::Clear()
{
  for (auto * triple : m_templateTriples)
    delete triple;
  m_templateTriples.clear();

  m_templateItemsNamesToReplacementItemsAddrs.clear();
  m_priorityOrderedTemplateTriples.clear();
  m_priorityOrderedTemplateTriples.resize((size_t)ScTemplateTripleType::ScConstr3TypeCount);
}

bool ScTemplate::IsEmpty() const
{
  return m_templateTriples.empty();
}

size_t ScTemplate::Size() const
{
  return m_templateTriples.size();
}

bool ScTemplate::HasReplacement(std::string const & repl) const
{
  return m_templateItemsNamesToReplacementItemsPositions.find(repl)
         != m_templateItemsNamesToReplacementItemsPositions.cend();
}

bool ScTemplate::HasReplacement(ScAddr const & replAddr) const
{
  return m_templateItemsNamesToReplacementItemsPositions.find(std::to_string(replAddr.Hash()))
         != m_templateItemsNamesToReplacementItemsPositions.cend();
}

ScTemplate & ScTemplate::Triple(
    ScTemplateItem const & param1,
    ScTemplateItem const & param2,
    ScTemplateItem const & param3)
{
  size_t const replPos = m_templateTriples.size() * 3;
  m_templateTriples.emplace_back(new ScTemplateTriple(param1, param2, param3, m_templateTriples.size()));

  if (param2.HasName() && param2.m_name == param1.m_name)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "The second and the first items in sc-template have the same replacement name `"
            << param2.m_name
            << "`. You can't use equal replacement for the second item and source/target items in sc-template.");

  if (param2.HasName() && param2.m_name == param3.m_name)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "The second and the third items in sc-template have the same replacement name `"
            << param2.m_name
            << "`. You can't use equal replacement for the second item and source/target items in sc-template.");

  ScTemplateTriple * triple = m_templateTriples.back();

  for (size_t i = 0; i < 3; ++i)
  {
    ScTemplateItem & item = triple->m_values[i];

    if (item.IsAssign() && item.m_typeValue.HasConstancyFlag() && !item.m_typeValue.IsVar())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Specified sc-type for sc-template item "
              << (item.HasName() ? ("`" + item.m_name + "`") : "")
              << "is constant sc-type. You can only use variable sc-types for items in sc-template.");

    if (item.IsAddr() && !item.m_addrValue.IsValid())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Specified sc-address for sc-template item "
              << (item.HasName() ? ("`" + item.m_name + "`") : "")
              << "is invalid. You can't use invalid sc-addresses for items in sc-template.");

    if (!item.m_name.empty())
    {
      if (item.IsAddr())
        m_templateItemsNamesToReplacementItemsAddrs[item.m_name] = item.m_addrValue;
      else
      {
        auto const & found = m_templateItemsNamesToReplacementItemsAddrs.find(item.m_name);
        if (found != m_templateItemsNamesToReplacementItemsAddrs.cend())
          item.SetAddr(found->second);
      }

      if (item.IsType())
        m_templateItemsNamesToTypes[item.m_name] = item.m_typeValue;

      if (!item.IsReplacement())
      {
        if (!HasReplacement(item.m_name))
          m_templateItemsNamesToReplacementItemsPositions.insert({item.m_name, replPos + i});
      }

      /* Store type there, if replacement for any type.
       * That allows to use it before original type will be processed
       */
      size_t const tripleIdx = replPos / 3;
      ScTemplateItem const & valueType = m_templateTriples[tripleIdx]->m_values[i];

      if (valueType.IsType())
        item.m_typeValue = valueType.m_typeValue;
    }
  }

  ScTemplateTripleType const priority = GetPriority(triple);
  auto const pr = (size_t)priority;
  m_priorityOrderedTemplateTriples[pr].insert(triple->m_index);

  return *this;
}

ScTemplate & ScTemplate::Quintuple(
    ScTemplateItem const & param1,
    ScTemplateItem const & param2,
    ScTemplateItem const & param3,
    ScTemplateItem const & param4,
    ScTemplateItem const & param5)
{
  size_t const replPos = m_templateTriples.size() * 3;

  ScTemplateItem edgeCommonItem = param2;

  // check if relation edge has replacement
  if (edgeCommonItem.m_name.empty())
  {
    std::stringstream ss;
    ss << "_repl_" << replPos + 1;
    edgeCommonItem.m_name = ss.str();
  }

  Triple(param1, edgeCommonItem, param3);
  Triple(param5, param4, edgeCommonItem.m_name);

  return *this;
}

ScTemplate::ScTemplateTripleType ScTemplate::GetPriority(ScTemplateTriple * triple)
{
  ScTemplateItem const & item1 = triple->m_values[0];
  ScTemplateItem const & item2 = triple->m_values[1];
  ScTemplateItem const & item3 = triple->m_values[2];

  if (item2.IsFixed())
    return ScTemplate::ScTemplateTripleType::AFA;

  if (item1.IsFixed() && item3.IsFixed())
    return ScTemplate::ScTemplateTripleType::FAF;

  if (item3.IsFixed())
    return ScTemplate::ScTemplateTripleType::AAF;

  if (item1.IsFixed() && (!item3.m_typeValue.IsEdge() || item3.m_typeValue.IsUnknown()))
  {
    auto const & it = m_templateItemsNamesToTypes.find(item3.m_name);
    if (it != m_templateItemsNamesToTypes.cend() && !it->second.IsEdge() && !it->second.IsUnknown())
      return ScTemplate::ScTemplateTripleType::FAN;
  }

  if (item1.IsFixed())
    return ScTemplate::ScTemplateTripleType::FAE;

  return ScTemplate::ScTemplateTripleType::AAA;
}

// --------------------------------

ScTemplateResultItem::ScTemplateResultItem()
  : m_context(nullptr)
{
}

ScTemplateResultItem::ScTemplateResultItem(
    ScMemoryContext * context,
    ScAddrVector results,
    ScTemplate::ScTemplateItemsToReplacementsItemsPositions replacements)
  : m_context(context)
  , m_replacementConstruction(std::move(results))
  , m_templateItemsNamesToReplacementItemPositions(std::move(replacements))
{
}

ScTemplateResultItem::ScTemplateResultItem(
    ScMemoryContext * context,
    ScTemplate::ScTemplateItemsToReplacementsItemsPositions replacements)
  : m_context(context)
  , m_templateItemsNamesToReplacementItemPositions(std::move(replacements))
{
}

ScTemplateResultItem::~ScTemplateResultItem() = default;

ScTemplateResultItem::ScTemplateResultItem(ScTemplateResultItem const & otherItem)
  : ScTemplateResultItem(
        otherItem.m_context,
        otherItem.m_replacementConstruction,
        otherItem.m_templateItemsNamesToReplacementItemPositions)
{
}

ScTemplateResultItem & ScTemplateResultItem::operator=(ScTemplateResultItem const & otherItem)
{
  if (this == &otherItem)
    return *this;

  m_context = otherItem.m_context;
  m_replacementConstruction.assign(
      otherItem.m_replacementConstruction.cbegin(), otherItem.m_replacementConstruction.cend());
  m_templateItemsNamesToReplacementItemPositions = otherItem.m_templateItemsNamesToReplacementItemPositions;

  return *this;
}

bool ScTemplateResultItem::Get(ScAddr const & varAddr, ScAddr & outAddr) const noexcept
{
  ScAddr const & addr = GetAddrByVarAddr(varAddr);
  if (addr.IsValid())
  {
    outAddr = addr;
    return true;
  }

  outAddr = ScAddr::Empty;
  return false;
}

ScAddr ScTemplateResultItem::operator[](ScAddr const & varAddr) const noexcept(false)
{
  ScAddr const & addr = GetAddrByVarAddr(varAddr);
  if (addr.IsValid())
    return addr;

  SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Var=" << varAddr.Hash() << " not found in replacements");
}

bool ScTemplateResultItem::Get(std::string const & name, ScAddr & outAddr) const noexcept
{
  ScAddr const & addr = GetAddrByName(name);
  if (addr.IsValid())
  {
    outAddr = addr;
    return true;
  }

  outAddr = ScAddr::Empty;
  return false;
}

ScAddr ScTemplateResultItem::operator[](std::string const & name) const noexcept(false)
{
  ScAddr const & addr = GetAddrByName(name);
  if (addr.IsValid())
    return addr;

  SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Alias=`" << name << "` not found in replacements");
}

bool ScTemplateResultItem::Get(size_t index, ScAddr & outAddr) const noexcept
{
  if (index < Size())
  {
    outAddr = m_replacementConstruction[index];
    return true;
  }

  outAddr = ScAddr::Empty;
  return false;
}

ScAddr const & ScTemplateResultItem::operator[](size_t index) const noexcept(false)
{
  if (index < Size())
    return m_replacementConstruction[index];

  SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Index=" << index << " must be < size=" << Size());
}

bool ScTemplateResultItem::Has(std::string const & name) const noexcept
{
  ScAddr const & addr = GetAddrByName(name);
  return addr.IsValid();
}

bool ScTemplateResultItem::Has(ScAddr const & varAddr) const noexcept
{
  ScAddr const & addr = GetAddrByVarAddr(varAddr);
  return addr.IsValid();
}

size_t ScTemplateResultItem::Size() const noexcept
{
  return m_replacementConstruction.size();
}

ScAddrVector::const_iterator ScTemplateResultItem::begin() const
{
  return m_replacementConstruction.cbegin();
}

ScAddrVector::const_iterator ScTemplateResultItem::end() const
{
  return m_replacementConstruction.cend();
}

ScTemplate::ScTemplateItemsToReplacementsItemsPositions const & ScTemplateResultItem::GetReplacements() const noexcept
{
  return m_templateItemsNamesToReplacementItemPositions;
}

ScAddr ScTemplateResultItem::GetAddrByName(std::string const & name) const
{
  auto it = m_templateItemsNamesToReplacementItemPositions.find(name);
  if (it != m_templateItemsNamesToReplacementItemPositions.cend())
    return m_replacementConstruction[it->second];

  ScAddr const & addr = m_context->HelperFindBySystemIdtf(name);
  if (addr.IsValid())
  {
    it = m_templateItemsNamesToReplacementItemPositions.find(std::to_string(addr.Hash()));
    if (it != m_templateItemsNamesToReplacementItemPositions.cend())
      return m_replacementConstruction[it->second];
  }

  return ScAddr::Empty;
}

ScAddr ScTemplateResultItem::GetAddrByVarAddr(ScAddr const & varAddr) const
{
  if (!varAddr.IsValid())
    return ScAddr::Empty;

  auto it = m_templateItemsNamesToReplacementItemPositions.find(std::to_string(varAddr.Hash()));
  if (it != m_templateItemsNamesToReplacementItemPositions.cend())
    return m_replacementConstruction[it->second];

  std::string const & varIdtf = m_context->HelperGetSystemIdtf(varAddr);
  it = m_templateItemsNamesToReplacementItemPositions.find(varIdtf);
  if (it != m_templateItemsNamesToReplacementItemPositions.cend())
    return m_replacementConstruction[it->second];

  return ScAddr::Empty;
}

// --------------------------------

ScTemplateSearchResult::ScTemplateSearchResult() noexcept = default;

size_t ScTemplateSearchResult::Size() const noexcept
{
  return m_replacementConstructions.size();
}

bool ScTemplateSearchResult::IsEmpty() const noexcept
{
  return Size() == 0;
}

bool ScTemplateSearchResult::Get(size_t index, ScTemplateResultItem & outItem) const noexcept
{
  if (index < Size())
  {
    outItem.m_context = m_context;
    outItem.m_templateItemsNamesToReplacementItemPositions = m_templateItemsNamesToReplacementItemsPositions;
    outItem.m_replacementConstruction.assign(
        m_replacementConstructions[index].cbegin(), m_replacementConstructions[index].cend());
    return true;
  }

  return false;
}

ScTemplateResultItem ScTemplateSearchResult::operator[](size_t index) const noexcept(false)
{
  if (index < Size())
    return {m_context, m_replacementConstructions[index], m_templateItemsNamesToReplacementItemsPositions};

  SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Index=" << index << " must be < size=" << Size());
}

void ScTemplateSearchResult::Clear() noexcept
{
  m_replacementConstructions.clear();
  m_templateItemsNamesToReplacementItemsPositions.clear();
}

ScTemplate::ScTemplateItemsToReplacementsItemsPositions ScTemplateSearchResult::GetReplacements() const noexcept
{
  ScTemplate::ScTemplateItemsToReplacementsItemsPositions replacementsItemsPositions;

  for (auto const & item : m_templateItemsNamesToReplacementItemsPositions)
  {
    replacementsItemsPositions.insert(item);

    std::stringstream ss(item.first);
    sc_addr_hash hash;
    ss >> hash;
    if (ss.fail() || !ss.eof())
      continue;

    ScAddr const & varAddr = ScAddr(hash);
    std::string const & sysIdtf = m_context->HelperGetSystemIdtf(varAddr);
    if (sysIdtf.empty())
      continue;

    replacementsItemsPositions.insert({sysIdtf, item.second});
  }

  return replacementsItemsPositions;
}
