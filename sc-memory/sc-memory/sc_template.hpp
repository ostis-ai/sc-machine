/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <utility>

#include "sc_addr.hpp"
#include "sc_type.hpp"
#include "sc_utils.hpp"

struct ScTemplateItemValue
{
  enum class Type : uint8_t
  {
    None,
    Type,
    Addr,
    Replace
  };

  ScTemplateItemValue()
  {
    m_itemType = Type::Type;
  }

  ScTemplateItemValue(ScAddr const & addr, char const * replName = nullptr)
  {
    SetAddr(addr, replName);
  }

  ScTemplateItemValue(ScType const & type, char const * replName = nullptr)
  {
    SetType(type, replName);
  }

  ScTemplateItemValue(char const * name)
  {
    SetReplacement(name);
  }

  ScTemplateItemValue(std::string const & name)
  {
    SetReplacement(name.c_str());
  }

  inline bool IsAddr() const
  {
    return (m_itemType == Type::Addr);
  }

  inline bool IsReplacement() const
  {
    return (m_itemType == Type::Replace);
  }

  inline bool IsType() const
  {
    return (m_itemType == Type::Type);
  }

  inline bool IsFixed() const
  {
    return IsAddr() || (IsReplacement() && m_addrValue.IsValid());
  }

  inline bool IsAssign() const
  {
    return m_itemType == Type::Type;
  }

  inline bool IsItemType(Type type) const
  {
    return (m_itemType == type);
  }

  void SetAddr(ScAddr const & addr, char const * replName = nullptr)
  {
    m_itemType = Type::Addr;
    m_addrValue = addr;
    if (replName)
      m_replacementName = replName;
  }

  void SetType(ScType const & type, char const * replName = nullptr)
  {
    m_itemType = Type::Type;
    m_typeValue = type;
    if (replName)
      m_replacementName = replName;
  }

  void SetReplacement(char const * name)
  {
    m_itemType = Type::Replace;
    if (name)
      m_replacementName = name;
  }

  SC_DEPRECATED(0.3.0, "Use ScTemplateItemValue::isFixed instead")
  bool CanBeAddr() const
  {
    return m_itemType == Type::Addr || m_itemType == Type::Replace;
  }

  SC_DEPRECATED(0.3.0, "Use ScTemplateItemValue::isAssign instead")
  bool CanBeType() const
  {
    return m_itemType == Type::Type;
  }

  Type m_itemType;

  ScAddr m_addrValue;
  ScType m_typeValue;
  std::string m_replacementName;
};

class ScTemplateConstr3
{
  friend class ScTemplate;

public:
  using ItemsArray = std::array<ScTemplateItemValue, 3>;

  enum class Flag : uint8_t
  {
    Required,
    NotRequired
  };

  ScTemplateConstr3(
      ScTemplateItemValue const & param1,
      ScTemplateItemValue const & param2,
      ScTemplateItemValue const & param3,
      size_t idx,
      Flag isRequired)
    : m_index(idx)
    , m_isRequired(isRequired)
  {
    m_values[0] = param1;
    m_values[1] = param2;
    m_values[2] = param3;
  }

  ScTemplateConstr3(ScTemplateConstr3 const & other)
    : m_index(other.m_index)
    , m_isRequired(other.m_isRequired)
  {
    m_values[0] = other.m_values[0];
    m_values[1] = other.m_values[1];
    m_values[2] = other.m_values[2];
  }

  bool operator==(ScTemplateConstr3 const & other) const
  {
    return this->m_index == other.m_index;
  }

  ItemsArray const & GetValues() const
  {
    return m_values;
  }

  void SetAddr(size_t idx, ScAddr const & addr)
  {
    m_values[idx].SetAddr(addr);
  }

  size_t CountFixed() const
  {
    return CountCommonT([](ScTemplateItemValue const & value) {
      return value.IsFixed();
    });
  }

  size_t CountAddrs() const
  {
    return CountCommonT([](ScTemplateItemValue const & value) {
      return value.IsAddr();
    });
  }
  size_t CountReplacements() const
  {
    return CountCommonT([](ScTemplateItemValue const & value) {
      return value.IsReplacement();
    });
  }

  template <typename Func>
  size_t CountCommonT(Func f) const
  {
    size_t result = 0;
    for (auto & v : m_values)
    {
      if (f(v))
        ++result;
    }
    return result;
  }

  bool IsRequired() const
  {
    return m_isRequired == Flag::Required;
  }

  /* Store original index in template. Because when perform search or generation
   * we sort triples in suitable for operation order.
   * Used to construct result
   */
  size_t m_index;

protected:
  ItemsArray m_values;
  Flag m_isRequired;
};

template <typename HashType>
struct ScTemplateConstr3HashFunc
{
  HashType operator()(std::reference_wrapper<ScTemplateConstr3> const & constr);
};

template <>
struct ScTemplateConstr3HashFunc<size_t>
{
  size_t operator()(std::reference_wrapper<ScTemplateConstr3> const & constr) const
  {
    return constr.get().m_index;
  }
};

struct ScTemplateConstr3EqualFunc
{
  bool operator()(std::reference_wrapper<ScTemplateConstr3> const & a, std::reference_wrapper<ScTemplateConstr3> const & b) const
  {
    return a.get().m_index, b.get().m_index;
  }
};

_SC_EXTERN ScTemplateItemValue operator>>(ScAddr const & value, char const * replName);
_SC_EXTERN ScTemplateItemValue operator>>(ScAddr const & value, std::string const & replName);
_SC_EXTERN ScTemplateItemValue operator>>(ScType const & value, char const * replName);
_SC_EXTERN ScTemplateItemValue operator>>(ScType const & value, std::string const & replName);

class ScTemplateGenResult;
class ScTemplateSearchResult;

enum class ScTemplateResultCode : uint8_t
{
  Success = 0,
  InvalidParams = 1,
  InternalError = 2
};

/* Parameters for template generator.
 * Can be used to replace variables by values
 */
class ScTemplateParams
{
  friend class ScTemplateGenerator;

public:
  ScTemplateParams & operator=(ScTemplateParams const & other) = delete;

  explicit ScTemplateParams() = default;

  SC_DEPRECATED(0.4.0, "You should to use ScTemplateParams::Add")
  _SC_EXTERN ScTemplateParams & add(std::string const & varIdtf, ScAddr const & value)
  {
    return Add(varIdtf, value);
  }

  _SC_EXTERN ScTemplateParams & Add(std::string const & varIdtf, ScAddr const & value)
  {
    assert(m_values.find(varIdtf) == m_values.end());
    m_values[varIdtf] = value;
    return *this;
  }

  _SC_EXTERN bool Get(std::string const & varIdtf, ScAddr & outResult) const
  {
    auto const it = m_values.find(varIdtf);
    if (it != m_values.end())
    {
      outResult = it->second;
      return true;
    }

    return false;
  }

  SC_DEPRECATED(0.4.0, "You should to use ScTemplateParams::IsEmpty")
  _SC_EXTERN bool empty() const
  {
    return m_values.empty();
  }

  _SC_EXTERN bool IsEmpty() const
  {
    return m_values.empty();
  }

  _SC_EXTERN static const ScTemplateParams Empty;

protected:
  using ParamsMap = std::map<std::string, ScAddr>;
  ParamsMap m_values;
};

enum class ScConstr3Type : uint8_t
{
  Foreground = 0,
  AFA = 1,
  FAF = 2,
  AAF = 3,
  FAE = 4,
  FAN = 5,
  AAA = 6,

  ScConstr3TypeCount
};

class ScTemplate final
{
  friend class ScMemoryContext;
  friend class ScTemplateSearch;
  friend class ScTemplateGenerator;
  friend class ScTemplateBuilder;
  friend class ScTemplateBuilderFromScs;

public:
  using TripleFlag = ScTemplateConstr3::Flag;

  class Result
  {
  public:
    explicit Result(bool result, std::string errorMsg = "")
      : m_result(result)
      , m_msg(std::move(errorMsg))
    {
    }

    operator bool() const
    {
      return m_result;
    }

    std::string const & Msg() const
    {
      return m_msg;
    }

  private:
    bool m_result = false;
    std::string m_msg;
  };

public:
  ScTemplate(ScTemplate const & other) = delete;
  ScTemplate & operator=(ScTemplate const & other) = delete;

  using ReplacementsMap = std::map<std::string, size_t>;
  using TemplateConstr3Vector = std::vector<ScTemplateConstr3>;
  using ProcessOrder = std::vector<size_t>;

  /*  If forceOrder flag is true, then search will be run in the same order,
   * that was used for a triples append
   */
  _SC_EXTERN explicit ScTemplate(bool forceOrder = false);

  _SC_EXTERN ScTemplate & operator()(
      ScTemplateItemValue const & param1,
      ScTemplateItemValue const & param2,
      ScTemplateItemValue const & param3,
      TripleFlag isRequired = TripleFlag::Required);

  _SC_EXTERN ScTemplate & operator()(
      ScTemplateItemValue const & param1,
      ScTemplateItemValue const & param2,
      ScTemplateItemValue const & param3,
      ScTemplateItemValue const & param4,
      ScTemplateItemValue const & param5,
      TripleFlag isRequired = TripleFlag::Required);

  _SC_EXTERN void Clear();
  _SC_EXTERN bool IsEmpty() const;

  _SC_EXTERN bool HasReplacement(std::string const & repl) const;

  /** Add construction:
   *          param2
   * param1 ----------> param3
   */
  _SC_EXTERN ScTemplate & Triple(
      ScTemplateItemValue const & param1,
      ScTemplateItemValue const & param2,
      ScTemplateItemValue const & param3,
      TripleFlag isRequired = TripleFlag::Required);

  /** Adds template:
   *           param2
   *	param1 ---------> param3
   *             ^
   *             |
   *             | param4
   *             |
   *           param5
   * Equal to two calls of triple, so this template add 6 items to result (NOT 5), to minimize
   * possible abuse, use result name mapping, and get result by names
   */
  _SC_EXTERN ScTemplate & TripleWithRelation(
      ScTemplateItemValue const & param1,
      ScTemplateItemValue const & param2,
      ScTemplateItemValue const & param3,
      ScTemplateItemValue const & param4,
      ScTemplateItemValue const & param5,
      TripleFlag isRequired = TripleFlag::Required);

protected:
  // Begin: calls by memory context
  Result Generate(
      ScMemoryContext & ctx,
      ScTemplateGenResult & result,
      ScTemplateParams const & params,
      ScTemplateResultCode * errorCode = nullptr) const;
  Result Search(ScMemoryContext & ctx, ScTemplateSearchResult & result);
  Result SearchInStruct(ScMemoryContext & ctx, ScAddr const & scStruct, ScTemplateSearchResult & result);

  // Builds template based on template in sc-memory
  Result FromScTemplate(
      ScMemoryContext & ctx,
      ScAddr const & scTemplateAddr,
      const ScTemplateParams & params = ScTemplateParams());
  Result FromScs(ScMemoryContext & ctx, std::string const & scsText);
  // End: calls by memory context

protected:
  // Store mapping of name to index in result vector
  ReplacementsMap m_replacements;
  // Store construction (triples)
  TemplateConstr3Vector m_constructions;
  ScAddrVector m_constants;

  using ScTemplateGroupedConstructions = std::unordered_set<std::reference_wrapper<ScTemplateConstr3>, ScTemplateConstr3HashFunc<size_t>, ScTemplateConstr3EqualFunc>;
  std::vector<ScTemplateGroupedConstructions> m_orderedConstructions;
  std::unordered_set<std::string> m_itemsNames;

  /* Caches (used to prevent processing order update on each search/gen)
   * Caches are mutable, to prevent changes of template in search and generation, they can access just a cache.
   * That because template passed into them by const reference.
   */
  mutable ProcessOrder m_searchCachedOrder;

  ScConstr3Type GetPriority(ScTemplateConstr3 const & constr);
};

class ScTemplateGenResult
{
  friend class ScTemplateGenerator;
  friend class ScSet;

public:
  ScTemplateGenResult() = default;

  ScAddr const & operator[](std::string const & name) const
  {
    auto const it = m_replacements.find(name);
    if (it != m_replacements.end())
    {
      if (it->second >= m_result.size())
        SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Invalid replacement " + name);

      return m_result[it->second];
    }
    else
    {
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Can't find replacement " + name);
    }
  }

  SC_DEPRECATED(0.3.0, "Use ScTemplateGenResult::Size instead")
  inline size_t GetSize() const
  {
    return m_result.size();
  }

  inline size_t Size() const
  {
    return m_result.size();
  }

  ScAddr const & operator[](size_t index)
  {
    SC_ASSERT(index < Size(), ());
    return m_result[index];
  }

  inline ScTemplate::ReplacementsMap const & GetReplacements() const
  {
    return m_replacements;
  }

protected:
  ScAddrVector m_result;

  ScTemplate::ReplacementsMap m_replacements;
};

class ScTemplateSearchResultItem
{
  friend class ScTemplateSearch;
  friend class ScTemplateSearchResult;

public:
  ScTemplateSearchResultItem(ScAddrVector const * results, ScTemplate::ReplacementsMap const * replacements)
    : m_results(results)
    , m_replacements(replacements)
  {
  }

  inline ScAddr const & operator[](std::string const & name) const
  {
    auto const it = m_replacements->find(name);
    SC_ASSERT(it != m_replacements->end(), ());
    SC_ASSERT(it->second < m_results->size(), ());
    return (*m_results)[it->second];
  }

  inline ScAddr const & operator[](size_t index) const
  {
    SC_ASSERT(index < Size(), ());
    return (*m_results)[index];
  }

  inline bool Has(std::string const & name) const
  {
    return (m_replacements->find(name) != m_replacements->end());
  }

  inline size_t Size() const
  {
    SC_ASSERT(m_results != nullptr, ());
    return m_results->size();
  }

protected:
  ScAddrVector const * m_results;
  ScTemplate::ReplacementsMap const * m_replacements;
};

class ScTemplateSearchResult
{
  friend class ScTemplateSearch;

public:
  inline size_t Size() const
  {
    return m_results.size();
  }

  inline bool IsEmpty() const
  {
    return Size() == 0;
  }

  inline bool GetResultItemSafe(size_t idx, ScTemplateSearchResultItem & outItem) const
  {
    if (idx < m_results.size())
    {
      outItem.m_replacements = &m_replacements;
      outItem.m_results = &(m_results[idx]);
      return true;
    }

    return false;
  }

  inline ScTemplateSearchResultItem operator[](size_t idx) const
  {
    SC_ASSERT(idx < m_results.size(), ());
    return {&(m_results[idx]), &m_replacements};
  }

  inline void Clear()
  {
    m_results.clear();
    m_replacements.clear();
  }

  inline ScTemplate::ReplacementsMap const & GetReplacements() const
  {
    return m_replacements;
  }

  template <typename FnT>
  void ForEach(FnT && f)
  {
    for (auto const & res : m_results)
      f(ScTemplateSearchResultItem(&res, &m_replacements));
  }

protected:
  using SearchResults = std::vector<ScAddrVector>;
  SearchResults m_results;
  ScTemplate::ReplacementsMap m_replacements;
};
