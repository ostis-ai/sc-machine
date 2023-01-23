/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <utility>
#include <functional>

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
    else if (m_replacementName.empty())
      m_replacementName = std::to_string(addr.Hash());
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

  Type m_itemType;

  ScAddr m_addrValue;
  ScType m_typeValue;
  std::string m_replacementName;
};

class ScTemplateTriple
{
  friend class ScTemplate;

public:
  using ItemsArray = std::array<ScTemplateItemValue, 3>;

  enum class Flag : uint8_t
  {
    Required,
    NotRequired
  };

  ScTemplateTriple(
      ScTemplateItemValue const & param1,
      ScTemplateItemValue const & param2,
      ScTemplateItemValue const & param3,
      size_t index)
    : m_index(index)
  {
    m_values[0] = param1;
    m_values[1] = param2;
    m_values[2] = param3;
  }

  ItemsArray const & GetValues() const
  {
    return m_values;
  }

  ScTemplateItemValue const & operator[](size_t index) const
  {
    if (index < m_values.size())
    {
      return m_values[index];
    }

    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Index=" + std::to_string(index) + " must be < size=" + std::to_string(m_values.size()));
  }

  /* Store original index in template. Because when perform search or generation
   * we sort triples in suitable for operation order.
   * Used to triple result
   */
  size_t m_index;

protected:
  ItemsArray m_values;
};

_SC_EXTERN ScTemplateItemValue operator>>(ScAddr const & value, char const * replName);
_SC_EXTERN ScTemplateItemValue operator>>(ScAddr const & value, std::string const & replName);
_SC_EXTERN ScTemplateItemValue operator>>(ScType const & value, char const * replName);
_SC_EXTERN ScTemplateItemValue operator>>(ScType const & value, std::string const & replName);

class ScTemplateGenResult;
class ScTemplateSearchResult;
class ScTemplateSearchResultItem;

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
    if (m_values.find(varIdtf) != m_values.cend())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Alias=" + varIdtf + " is invalid");
    }

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
    return IsEmpty();
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

enum class ScTemplateTripleType : uint8_t
{
  Foreground = 0,
  AFA = 1,   // _... -> _...
  FAF = 2,   // ... _-> ...
  AAF = 3,   // _... _-> ...
  PFAE = 4,  // ... _-> ... or ... _-> _edge, triple with small count of output arcs for the first item element
  FAE = 5,   // ... _-> ...
  FAN = 6,   // ... _-> _edge
  AAA = 7,   // _... _-> _...

  ScConstr3TypeCount
};

enum class ScTemplateSearchRequest : uint8_t
{
  CONTINUE,
  STOP,
  ERROR
};

using ScTemplateSearchResultCallback = std::function<void(ScTemplateSearchResultItem const & resultItem)>;
using ScTemplateSearchResultCallbackWithRequest =
    std::function<ScTemplateSearchRequest(ScTemplateSearchResultItem const & resultItem)>;
using ScTemplateSearchResultCheckCallback =
    std::function<bool(ScAddr const & beginAddr, ScAddr const & edgeAddr, ScAddr const & endAddr)>;

class ScTemplate final
{
  friend class ScMemoryContext;
  friend class ScTemplateSearch;
  friend class ScTemplateGenerator;
  friend class ScTemplateBuilder;
  friend class ScTemplateBuilderFromScs;

public:
  using TripleFlag = ScTemplateTriple::Flag;

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

  using ReplacementsMap = std::unordered_multimap<std::string, size_t>;
  using ScTemplateTriplesVector = std::vector<ScTemplateTriple *>;

  /*  If forceOrder flag is true, then search will be run in the same order,
   * that was used for a triples append
   */
  _SC_EXTERN explicit ScTemplate(bool forceOrder = false);

  _SC_EXTERN ~ScTemplate()
  {
    for (auto * triple : m_triples)
      delete triple;
    m_triples.clear();
  }

  _SC_EXTERN ScTemplate & operator()(
      ScTemplateItemValue const & param1,
      ScTemplateItemValue const & param2,
      ScTemplateItemValue const & param3);

  _SC_EXTERN ScTemplate & operator()(
      ScTemplateItemValue const & param1,
      ScTemplateItemValue const & param2,
      ScTemplateItemValue const & param3,
      ScTemplateItemValue const & param4,
      ScTemplateItemValue const & param5);

  _SC_EXTERN void Clear();
  _SC_EXTERN bool IsEmpty() const;

  _SC_EXTERN bool HasReplacement(std::string const & repl) const;

  /** Add triple:
   *          param2
   * param1 ----------> param3
   */
  _SC_EXTERN ScTemplate & Triple(
      ScTemplateItemValue const & param1,
      ScTemplateItemValue const & param2,
      ScTemplateItemValue const & param3);

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
      ScTemplateItemValue const & param5);

protected:
  // Begin: calls by memory context
  Result Generate(
      ScMemoryContext & ctx,
      ScTemplateGenResult & result,
      ScTemplateParams const & params,
      ScTemplateResultCode * errorCode = nullptr) const;
  SC_DEPRECATED(
      0.7.1,
      "Use ScTemplate::Search(ScMemoryContext & ctx, ScTemplateSearchResultCallback const & callback, "
      "ScTemplateSearchResultCheckCallback const & checkCallback) instead.")
  Result Search(ScMemoryContext & ctx, ScTemplateSearchResult & result) const;
  void Search(
      ScMemoryContext & ctx,
      ScTemplateSearchResultCallback const & callback,
      ScTemplateSearchResultCheckCallback const & checkCallback) const;
  void Search(
      ScMemoryContext & ctx,
      ScTemplateSearchResultCallbackWithRequest const & callback,
      ScTemplateSearchResultCheckCallback const & checkCallback) const;

  SC_DEPRECATED(
      0.7.1,
      "Use ScTemplate::Search(ScMemoryContext & ctx, ScTemplateSearchResultCallback const & callback, "
      "ScTemplateSearchResultCheckCallback const & checkCallback) instead.")
  Result SearchInStruct(ScMemoryContext & ctx, ScAddr const & scStruct, ScTemplateSearchResult & result) const;

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
  // Store triples
  ScTemplateTriplesVector m_triples;

  using ScTemplateGroupedTriples = std::unordered_set<size_t>;
  std::vector<ScTemplateGroupedTriples> m_orderedTriples;
  std::map<std::string, ScAddr> m_namesToAddrs;
  std::map<std::string, ScType> m_namesToTypes;

  ScTemplateTripleType GetPriority(ScTemplateTriple * triple);
};

class ScTemplateGenResult
{
  friend class ScTemplateGenerator;
  friend class ScSet;

public:
  ScTemplateGenResult() = default;

  /* Gets generated sc-element address by replacement `name`.
   * @param name A replacement name of sc-element address
   * @param outAddr[out] A found sc-element address by replacement `name`
   * @returns true, if sc-element address found by replacement `name`, otherwise false
   */
  inline bool Get(std::string const & name, ScAddr & outAddr) const noexcept
  {
    auto const & it = m_replacements.find(name);
    if (it != m_replacements.cend())
    {
      outAddr = m_result[it->second];
      return true;
    }

    outAddr = ScAddr::Empty;
    return false;
  }

  /* Gets generated sc-element address by replacement `name`.
   * @param name A replacement name of sc-element address
   * @returns Generated sc-element address.
   * @throws utils::ExceptionInvalidParams if `name` is invalid or not found in replacements.
   */
  ScAddr const & operator[](std::string const & name) const noexcept(false)
  {
    auto const & it = m_replacements.find(name);
    if (it != m_replacements.cend())
    {
      if (it->second < Size())
      {
        return m_result[it->second];
      }

      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Alias=" + name + " is invalid");
    }

    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Alias=" + name + " not found in replacements");
  }

  /* Gets generated sc-element address by `index` in triple.
   * @param name A replacement name of sc-element address
   * @param outAddr[out] A found sc-element address by `index` in triple
   * @returns true, if sc-element address found by `index` in triple, otherwise false
   */
  inline bool Get(size_t index, ScAddr & outAddr) const noexcept
  {
    if (index < Size())
    {
      outAddr = m_result[index];
      return true;
    }

    outAddr = ScAddr::Empty;
    return false;
  }

  /* Gets generated sc-element address by `index` in triple.
   * @param name A replacement name of sc-element address
   * @returns Generated sc-element address.
   * @throws utils::ExceptionInvalidParams if `index` < generated construction size.
   */
  ScAddr const & operator[](size_t index) const noexcept(false)
  {
    if (index < Size())
    {
      return m_result[index];
    }

    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Index=" + std::to_string(index) + " must be < size=" + std::to_string(Size()));
  }

  SC_DEPRECATED(0.3.0, "Use ScTemplateGenResult::Size instead")
  inline size_t GetSize() const
  {
    return m_result.size();
  }

  //! Gets generated construction size
  inline size_t Size() const
  {
    return m_result.size();
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
  ScTemplateSearchResultItem()
    : m_results(nullptr)
    , m_replacements(nullptr)
  {
  }

  ScTemplateSearchResultItem(ScAddrVector const * results, ScTemplate::ReplacementsMap const * replacements)
    : m_results(results)
    , m_replacements(replacements)
  {
  }

  /* Gets found sc-element address by replacement `name`.
   * @param name A replacement name of sc-element address
   * @param outAddr[out] A found sc-element address by replacement `name`
   * @returns true, if sc-element address found by replacement `name`, otherwise false
   */
  inline bool Get(std::string const & name, ScAddr & outAddr) const noexcept
  {
    auto const & it = m_replacements->find(name);
    if (it != m_replacements->cend())
    {
      outAddr = (*m_results)[it->second];
      return true;
    }

    outAddr = ScAddr::Empty;
    return false;
  }

  /* Gets found sc-element address by replacement `name`.
   * @param name A replacement name of sc-element address
   * @returns Found sc-element address.
   * @throws utils::ExceptionInvalidParams if `name` is invalid or not found in replacements.
   */
  inline ScAddr const & operator[](std::string const & name) const noexcept(false)
  {
    auto const & it = m_replacements->find(name);
    if (it != m_replacements->cend())
    {
      return (*m_results)[it->second];
    }

    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Alias=" + name + " not found in replacements");
  }

  /* Gets found sc-element address by `index` in triple.
   * @param name A replacement name of sc-element address
   * @param outAddr[out] A found sc-element address by `index` in triple
   * @returns true, if sc-element address found by `index` in triple, otherwise false
   */
  inline bool Get(size_t index, ScAddr & outAddr) const noexcept
  {
    if (index < Size())
    {
      outAddr = (*m_results)[index];
      return true;
    }

    outAddr = ScAddr::Empty;
    return false;
  }

  /* Gets found sc-element address by `index` in triple.
   * @param name A replacement name of sc-element address
   * @returns Found sc-element address.
   * @throws utils::ExceptionInvalidParams if `index` < found construction size.
   */
  inline ScAddr const & operator[](size_t index) const noexcept(false)
  {
    if (index < Size())
    {
      return (*m_results)[index];
    }

    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Index=" + std::to_string(index) + " must be < size=" + std::to_string(Size()));
  }

  //! Checks if replacement `name` exists in replacements
  inline bool Has(std::string const & name) const
  {
    return m_replacements != nullptr && m_replacements->find(name) != m_replacements->cend();
  }

  //! Gets found construction size
  inline size_t Size() const
  {
    return m_results ? m_results->size() : 0;
  }

protected:
  ScAddrVector const * m_results;
  ScTemplate::ReplacementsMap const * m_replacements;
};

SC_DEPRECATED(
    0.7.1,
    "Use ScMemoryContext::HelperSearchTemplate(ScTemplate const & templ, ScTemplateSearchResultCallback const & "
    "callback, ScTemplateSearchResultCheckCallback const & checkCallback) instead.")
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

  SC_DEPRECATED(0.7.1, "Use ScTemplateSearchResult::Get(size_t index, ScTemplateSearchResultItem & outItem)")
  inline bool GetResultItemSafe(size_t index, ScTemplateSearchResultItem & outItem) const
  {
    return Get(index, outItem);
  }

  inline bool Get(size_t index, ScTemplateSearchResultItem & outItem) const
  {
    if (index < Size())
    {
      outItem.m_replacements = &m_replacements;
      outItem.m_results = &(m_results[index]);
      return true;
    }

    return false;
  }

  inline ScTemplateSearchResultItem operator[](size_t index) const
  {
    if (index < Size())
    {
      return {&(m_results[index]), &m_replacements};
    }

    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Index=" + std::to_string(index) + " must be < size=" + std::to_string(Size()));
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
