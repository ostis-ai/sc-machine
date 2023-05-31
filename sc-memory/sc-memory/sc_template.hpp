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

struct ScTemplateItem
{
  enum class Type : uint8_t
  {
    Type,
    Addr,
    Replace
  };

  ScTemplateItem()
  {
    m_itemType = Type::Type;
  }

  ScTemplateItem(ScAddr const & addr, char const * replName = nullptr)
  {
    SetAddr(addr, replName);
  }

  ScTemplateItem(ScType const & type, char const * replName = nullptr)
  {
    SetType(type, replName);
  }

  ScTemplateItem(char const * name)
  {
    SetReplacement(name);
  }

  ScTemplateItem(std::string const & name)
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

  void SetAddr(ScAddr const & addr, char const * replName = nullptr)
  {
    m_itemType = Type::Addr;
    m_addrValue = addr;
    if (replName)
      m_name = replName;
    else if (m_name.empty())
      m_name = std::to_string(addr.Hash());
  }

  void SetType(ScType const & type, char const * replName = nullptr)
  {
    m_itemType = Type::Type;
    m_typeValue = type;
    if (replName)
      m_name = replName;
  }

  void SetReplacement(char const * name)
  {
    m_itemType = Type::Replace;
    if (name)
      m_name = name;
  }

  Type m_itemType;

  ScAddr m_addrValue;
  ScType m_typeValue;
  std::string m_name;
};

//! backward compatibility
using ScTemplateItemValue = ScTemplateItem;

class ScTemplateTriple
{
  friend class ScTemplate;

public:
  SC_DISALLOW_COPY_AND_MOVE(ScTemplateTriple);

  using ScTemplateTripleItems = std::array<ScTemplateItem, 3>;

  ScTemplateTriple(
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

  ScTemplateTripleItems const & GetValues() const
  {
    return m_values;
  }

  ScTemplateItem const & operator[](size_t index) const noexcept(false)
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
  ScTemplateTripleItems m_values;
};

_SC_EXTERN ScTemplateItem operator>>(ScAddr const & value, char const * replName);
_SC_EXTERN ScTemplateItem operator>>(ScAddr const & value, std::string const & replName);
_SC_EXTERN ScTemplateItem operator>>(ScType const & value, char const * replName);
_SC_EXTERN ScTemplateItem operator>>(ScType const & value, std::string const & replName);

class ScTemplateResultItem;
class ScTemplateSearchResult;
class ScTemplateResultItem;

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
  using ScTemplateItemsToParams = std::map<std::string, ScAddr>;

  ScTemplateParams() = default;

  _SC_EXTERN ScTemplateParams & Add(std::string const & varIdtf, ScAddr const & value) noexcept(false)
  {
    if (m_templateItemsToParams.find(varIdtf) == m_templateItemsToParams.cend())
    {
      m_templateItemsToParams[varIdtf] = value;
      return *this;
    }

    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Alias=" + varIdtf + " is busy");
  }

  _SC_EXTERN bool Get(std::string const & varIdtf, ScAddr & outResult) const noexcept
  {
    auto const it = m_templateItemsToParams.find(varIdtf);
    if (it != m_templateItemsToParams.cend())
    {
      outResult = it->second;
      return true;
    }

    return false;
  }

  _SC_EXTERN ScTemplateItemsToParams GetAll() const
  {
    return m_templateItemsToParams;
  }

  _SC_EXTERN bool IsEmpty() const noexcept
  {
    return m_templateItemsToParams.empty();
  }

  _SC_EXTERN static const ScTemplateParams Empty;

protected:
  ScTemplateItemsToParams m_templateItemsToParams;
};

enum class ScTemplateTripleType : uint8_t
{
  AFA = 0,  // _... -> _...
  FAF = 1,  // ... _-> ...
  AAF = 2,  // _... _-> ...
  FAN = 3,  // ... _-> ...
  FAE = 4,  // ... _-> _edge
  AAA = 5,  // _... _-> _...

  ScConstr3TypeCount
};

enum class ScTemplateSearchRequest : uint8_t
{
  CONTINUE,
  STOP,
  ERROR
};

using ScTemplateSearchResultCallback = std::function<void(ScTemplateResultItem const & resultItem)>;
using ScTemplateSearchResultCallbackWithRequest =
    std::function<ScTemplateSearchRequest(ScTemplateResultItem const & resultItem)>;
using ScTemplateSearchResultFilterCallback = std::function<bool(ScTemplateResultItem const & resultItem)>;
using ScTemplateSearchResultCheckCallback = std::function<bool(ScAddr const & addr)>;

class ScTemplate final
{
  friend class ScMemoryContext;
  friend class ScTemplateSearch;
  friend class ScTemplateGenerator;
  friend class ScTemplateBuilder;
  friend class ScTemplateBuilderFromScs;

public:
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
  SC_DISALLOW_COPY_AND_MOVE(ScTemplate);

  using ScTemplateItemsToReplacementsItemsPositions = std::unordered_map<std::string, size_t>;
  using ScTemplateTriplesVector = std::vector<ScTemplateTriple *>;

  _SC_EXTERN explicit ScTemplate();

  SC_DEPRECATED(0.8.0, "Now ScTemplate sorts itself effectively")
  _SC_EXTERN explicit ScTemplate(bool forceOrder);

  _SC_EXTERN ~ScTemplate()
  {
    for (auto * triple : m_templateTriples)
      delete triple;
    m_templateTriples.clear();
  }

  _SC_EXTERN ScTemplate & operator()(
      ScTemplateItem const & param1,
      ScTemplateItem const & param2,
      ScTemplateItem const & param3) noexcept(false);

  _SC_EXTERN ScTemplate & operator()(
      ScTemplateItem const & param1,
      ScTemplateItem const & param2,
      ScTemplateItem const & param3,
      ScTemplateItem const & param4,
      ScTemplateItem const & param5) noexcept(false);

  _SC_EXTERN void Clear();
  _SC_EXTERN bool IsEmpty() const;

  _SC_EXTERN bool HasReplacement(std::string const & repl) const;

  /** Add triple:
   *          param2
   * param1 ----------> param3
   */
  _SC_EXTERN ScTemplate & Triple(
      ScTemplateItem const & param1,
      ScTemplateItem const & param2,
      ScTemplateItem const & param3) noexcept(false);

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
  _SC_EXTERN ScTemplate & Quintuple(
      ScTemplateItem const & param1,
      ScTemplateItem const & param2,
      ScTemplateItem const & param3,
      ScTemplateItem const & param4,
      ScTemplateItem const & param5) noexcept(false);

  SC_DEPRECATED(
      0.8.0,
      "Use ScTemplate::Quintuple(ScTemplateItem const & param1, ScTemplateItem const & param2, "
      "ScTemplateItem const & param3, ScTemplateItem const & param4, ScTemplateItem const & param5) "
      "noexcept(false) instead.")
  _SC_EXTERN ScTemplate & TripleWithRelation(
      ScTemplateItem const & param1,
      ScTemplateItem const & param2,
      ScTemplateItem const & param3,
      ScTemplateItem const & param4,
      ScTemplateItem const & param5) noexcept(false);

protected:
  // Begin: calls by memory context
  Result Generate(
      ScMemoryContext & ctx,
      ScTemplateResultItem & result,
      ScTemplateParams const & params,
      ScTemplateResultCode * errorCode = nullptr) const noexcept(false);
  SC_DEPRECATED(
      0.8.0,
      "Use ScTemplate::Search(ScMemoryContext & ctx, ScTemplateSearchResultCallback const & callback, "
      "ScTemplateSearchResultCheckCallback const & checkCallback) instead.")
  Result Search(ScMemoryContext & ctx, ScTemplateSearchResult & result) const noexcept(false);
  void Search(
      ScMemoryContext & ctx,
      ScTemplateSearchResultCallback const & callback,
      ScTemplateSearchResultFilterCallback const & filterCallback = {},
      ScTemplateSearchResultCheckCallback const & checkCallback = {}) const noexcept(false);
  void Search(
      ScMemoryContext & ctx,
      ScTemplateSearchResultCallbackWithRequest const & callback,
      ScTemplateSearchResultFilterCallback const & filterCallback = {},
      ScTemplateSearchResultCheckCallback const & checkCallback = {}) const noexcept(false);

  SC_DEPRECATED(
      0.8.0,
      "Use ScTemplate::Search(ScMemoryContext & ctx, ScTemplateSearchResultCallback const & callback, "
      "ScTemplateSearchResultCheckCallback const & checkCallback) instead.")
  Result SearchInStruct(ScMemoryContext & ctx, ScAddr const & scStruct, ScTemplateSearchResult & result) const
      noexcept(false);

  // Builds template based on template in sc-memory
  Result FromScTemplate(
      ScMemoryContext & ctx,
      ScAddr const & scTemplateAddr,
      const ScTemplateParams & params = ScTemplateParams()) noexcept(false);
  Result FromScs(ScMemoryContext & ctx, std::string const & scsText) noexcept(false);
  // End: calls by memory context

protected:
  // Store mapping of name to index in result vector
  ScTemplateItemsToReplacementsItemsPositions m_templateItemsNamesToReplacementItemsPositions;
  // Store triples
  ScTemplateTriplesVector m_templateTriples;

  using ScTemplateGroupedTriples = std::unordered_set<size_t>;
  std::vector<ScTemplateGroupedTriples> m_priorityOrderedTemplateTriples;
  std::map<std::string, ScAddr> m_templateItemsNamesToReplacementItemsAddrs;
  std::map<std::string, ScType> m_templateItemsNamesToTypes;

  ScTemplateTripleType GetPriority(ScTemplateTriple * triple);
};

class ScTemplateResultItem
{
  friend class ScTemplateGenerator;
  friend class ScSet;
  friend class ScTemplateSearch;
  friend class ScTemplateSearchResult;

public:
  ScTemplateResultItem()
    : m_context(nullptr)
  {
  }

  ScTemplateResultItem(
      sc_memory_context const * context,
      ScAddrVector results,
      ScTemplate::ScTemplateItemsToReplacementsItemsPositions replacements)
    : m_context(context)
    , m_replacementConstruction(std::move(results))
    , m_templateItemsNamesToReplacementItemPositions(std::move(replacements))
  {
  }

  ScTemplateResultItem(
      sc_memory_context const * context,
      ScTemplate::ScTemplateItemsToReplacementsItemsPositions replacements)
    : m_context(context)
    , m_templateItemsNamesToReplacementItemPositions(std::move(replacements))
  {
  }

  SC_DEPRECATED(0.8.0, "Use ScTemplateSearchResultItem::ScTemplateSearchResultItem() instead of")
  ScTemplateResultItem(ScAddrVector * results, ScTemplate::ScTemplateItemsToReplacementsItemsPositions * replacements)
    : m_context(nullptr)
  {
  }

  ScTemplateResultItem(ScTemplateResultItem const & otherItem)
    : ScTemplateResultItem(
          otherItem.m_context,
          otherItem.m_replacementConstruction,
          otherItem.m_templateItemsNamesToReplacementItemPositions)
  {
  }

  ScTemplateResultItem & operator=(ScTemplateResultItem const & otherItem)
  {
    if (this == &otherItem)
      return *this;

    m_context = otherItem.m_context;
    m_replacementConstruction.assign(
        otherItem.m_replacementConstruction.cbegin(), otherItem.m_replacementConstruction.cend());
    m_templateItemsNamesToReplacementItemPositions = otherItem.m_templateItemsNamesToReplacementItemPositions;

    return *this;
  }

  /* Gets found sc-element address by `varAddr`.
   * @param varAddr A template var sc-element address
   * @param outAddr[out] A found sc-element address by `varAddr`
   * @returns true, if sc-element address found by `varAddr`, otherwise false
   */
  inline bool Get(ScAddr const & varAddr, ScAddr & outAddr) const noexcept
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

  /* Gets found sc-element address by `varAddr`.
   * @param varAddr A template var sc-element address
   * @returns Found sc-element address.
   * @throws utils::ExceptionInvalidParams if `varAddr` is invalid or not found in replacements.
   */
  inline ScAddr operator[](ScAddr const & varAddr) const noexcept(false)
  {
    ScAddr const & addr = GetAddrByVarAddr(varAddr);
    if (addr.IsValid())
      return addr;

    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Var=" << varAddr.Hash() << " not found in replacements");
  }

  /* Gets found sc-element address by replacement `name`.
   * @param name A replacement name of sc-element address
   * @param outAddr[out] A found sc-element address by replacement `name`
   * @returns true, if sc-element address found by replacement `name`, otherwise false
   */
  inline bool Get(std::string const & name, ScAddr & outAddr) const noexcept
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

  /* Gets found sc-element address by replacement `name`.
   * @param name A replacement name of sc-element address
   * @returns Found sc-element address.
   * @throws utils::ExceptionInvalidParams if `name` is invalid or not found in replacements.
   */
  inline ScAddr operator[](std::string const & name) const noexcept(false)
  {
    ScAddr const & addr = GetAddrByName(name);
    if (addr.IsValid())
      return addr;

    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Alias=" << name << " not found in replacements");
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
      outAddr = m_replacementConstruction[index];
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
      return m_replacementConstruction[index];

    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Index=" << index << " must be < size=" << Size());
  }

  //! Checks if replacement `name` exists in replacements
  inline bool Has(std::string const & name) const noexcept
  {
    ScAddr const & addr = GetAddrByName(name);
    return addr.IsValid();
  }

  //! Checks if `varAddr` exists in replacements
  inline bool Has(ScAddr const & varAddr) const noexcept
  {
    ScAddr const & addr = GetAddrByVarAddr(varAddr);
    return addr.IsValid();
  }

  //! Gets found construction size
  inline size_t Size() const noexcept
  {
    return m_replacementConstruction.size();
  }

  ScAddrVector::const_iterator begin() const
  {
    return m_replacementConstruction.cbegin();
  }

  ScAddrVector::const_iterator end() const
  {
    return m_replacementConstruction.cend();
  }

  inline ScTemplate::ScTemplateItemsToReplacementsItemsPositions const & GetReplacements() const noexcept
  {
    return m_templateItemsNamesToReplacementItemPositions;
  }

  ~ScTemplateResultItem() = default;

protected:
  ScAddr GetAddrByName(std::string const & name) const
  {
    auto it = m_templateItemsNamesToReplacementItemPositions.find(name);
    if (it != m_templateItemsNamesToReplacementItemPositions.cend())
      return m_replacementConstruction[it->second];

    ScAddr const & addr = GetAddrBySystemIdtf(name);
    if (addr.IsValid())
    {
      it = m_templateItemsNamesToReplacementItemPositions.find(std::to_string(addr.Hash()));
      if (it != m_templateItemsNamesToReplacementItemPositions.cend())
        return m_replacementConstruction[it->second];
    }

    return ScAddr::Empty;
  }

  ScAddr GetAddrByVarAddr(ScAddr const & varAddr) const
  {
    if (!varAddr.IsValid())
      return ScAddr::Empty;

    auto it = m_templateItemsNamesToReplacementItemPositions.find(std::to_string(varAddr.Hash()));
    if (it != m_templateItemsNamesToReplacementItemPositions.cend())
      return m_replacementConstruction[it->second];

    std::string const & varIdtf = GetSystemIdtfByAddr(varAddr);
    it = m_templateItemsNamesToReplacementItemPositions.find(varIdtf);
    if (it != m_templateItemsNamesToReplacementItemPositions.cend())
      return m_replacementConstruction[it->second];

    return ScAddr::Empty;
  }

  ScAddr GetAddrBySystemIdtf(std::string const & name) const;

  std::string GetSystemIdtfByAddr(ScAddr const & addr) const;

  sc_memory_context const * m_context;

  ScAddrVector m_replacementConstruction;
  ScTemplate::ScTemplateItemsToReplacementsItemsPositions m_templateItemsNamesToReplacementItemPositions;
};

using ScTemplateGenResult = ScTemplateResultItem;
using ScTemplateSearchResultItem = ScTemplateResultItem;

class SC_DEPRECATED(
    0.8.0,
    "Use callback-based ScMemoryContext::HelperSearchTemplate(ScTemplate const & templ, ScTemplateSearchResultCallback "
    "const & callback, ScTemplateSearchResultCheckCallback const & checkCallback) instead.") ScTemplateSearchResult
{
  friend class ScTemplateSearch;

public:
  ScTemplateSearchResult() = default;

  inline size_t Size() const
  {
    return m_replacementConstructions.size();
  }

  inline bool IsEmpty() const
  {
    return Size() == 0;
  }

  SC_DEPRECATED(0.8.0, "Use ScTemplateSearchResult::Get(size_t index, ScTemplateResultItem & outItem)")
  inline bool GetResultItemSafe(size_t index, ScTemplateResultItem & outItem) const noexcept
  {
    return Get(index, outItem);
  }

  inline bool Get(size_t index, ScTemplateResultItem & outItem) const noexcept
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

  inline ScTemplateResultItem operator[](size_t index) const noexcept(false)
  {
    if (index < Size())
      return {m_context, m_replacementConstructions[index], m_templateItemsNamesToReplacementItemsPositions};

    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Index=" << index << " must be < size=" << Size());
  }

  inline void Clear() noexcept
  {
    m_replacementConstructions.clear();
    m_templateItemsNamesToReplacementItemsPositions.clear();
  }

  inline ScTemplate::ScTemplateItemsToReplacementsItemsPositions const & GetReplacements() const noexcept
  {
    return m_templateItemsNamesToReplacementItemsPositions;
  }

  template <typename FnT>
  void ForEach(FnT && f) noexcept
  {
    for (auto & res : m_replacementConstructions)
      f(ScTemplateResultItem{m_context, res, m_templateItemsNamesToReplacementItemsPositions});
  }

protected:
  sc_memory_context const * m_context;

  using SearchResults = std::vector<ScAddrVector>;
  SearchResults m_replacementConstructions;
  ScTemplate::ScTemplateItemsToReplacementsItemsPositions m_templateItemsNamesToReplacementItemsPositions;
};
