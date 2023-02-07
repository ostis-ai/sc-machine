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
    None,
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

  inline bool IsItemType(Type type) const
  {
    return (m_itemType == type);
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

  ScTemplateItem const & operator[](size_t index) const
  {
    if (index < m_values.size())
    {
      return m_values[index];
    }

    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Index=" + std::to_string(index) + " must be < size=" + std::to_string(m_values.size()));
  }

  void ForEach(std::function<void(ScTemplateItem const & item, size_t const index)> const & callback)
  {
    callback(m_values[0], 0);
    callback(m_values[1], 1);
    callback(m_values[2], 2);
  }

  bool AnyOf(std::function<bool(ScTemplateItem const & item, size_t const index)> const & callback)
  {
    if (callback(m_values[1], 1))
      return true;

    if (callback(m_values[0], 0))
      return true;

    if (callback(m_values[2], 2))
      return true;

    return false;
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
    if (m_templateItemsToParams.find(varIdtf) != m_templateItemsToParams.cend())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Alias=" + varIdtf + " is invalid");
    }

    m_templateItemsToParams[varIdtf] = value;
    return *this;
  }

  _SC_EXTERN bool Get(std::string const & varIdtf, ScAddr & outResult) const
  {
    auto const it = m_templateItemsToParams.find(varIdtf);
    if (it != m_templateItemsToParams.end())
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
    return m_templateItemsToParams.empty();
  }

  _SC_EXTERN static const ScTemplateParams Empty;

protected:
  using ScTemplateItemsToParams = std::map<std::string, ScAddr>;
  ScTemplateItemsToParams m_templateItemsToParams;
};

enum class ScTemplateTripleType : uint8_t
{
  AFA = 0,  // _... -> _...
  FAF = 1,  // ... _-> ...
  AAF = 2,  // _... _-> ...
  FAE = 3,  // ... _-> ...
  FAN = 4,  // ... _-> _edge
  AAA = 5,  // _... _-> _...

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
using ScTemplateSearchResultFilterCallback =
    std::function<bool(ScTemplateSearchResultItem const & resultItem)>;
using ScTemplateSearchResultCheckCallback =
    std::function<bool(ScAddr const & addr)>;

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
  ScTemplate(ScTemplate const & other) = delete;
  ScTemplate & operator=(ScTemplate const & other) = delete;

  using ScTemplateItemsToReplacementsItemsPositions = std::unordered_multimap<std::string, size_t>;
  using ScTemplateTriplesVector = std::vector<ScTemplateTriple *>;

  _SC_EXTERN explicit ScTemplate(bool forceOrder = false);

  _SC_EXTERN ~ScTemplate()
  {
    for (auto * triple : m_templateTriples)
      delete triple;
    m_templateTriples.clear();
  }

  _SC_EXTERN ScTemplate & operator()(
      ScTemplateItem const & param1,
      ScTemplateItem const & param2,
      ScTemplateItem const & param3);

  _SC_EXTERN ScTemplate & operator()(
      ScTemplateItem const & param1,
      ScTemplateItem const & param2,
      ScTemplateItem const & param3,
      ScTemplateItem const & param4,
      ScTemplateItem const & param5);

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
      ScTemplateItem const & param3);

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
      ScTemplateItem const & param1,
      ScTemplateItem const & param2,
      ScTemplateItem const & param3,
      ScTemplateItem const & param4,
      ScTemplateItem const & param5);

protected:
  // Begin: calls by memory context
  Result Generate(
      ScMemoryContext & ctx,
      ScTemplateGenResult & result,
      ScTemplateParams const & params,
      ScTemplateResultCode * errorCode = nullptr) const;
  SC_DEPRECATED(
      0.8.0,
      "Use ScTemplate::Search(ScMemoryContext & ctx, ScTemplateSearchResultCallback const & callback, "
      "ScTemplateSearchResultCheckCallback const & checkCallback) instead.")
  Result Search(ScMemoryContext & ctx, ScTemplateSearchResult & result) const;
  void Search(
      ScMemoryContext & ctx,
      ScTemplateSearchResultCallback const & callback,
      ScTemplateSearchResultFilterCallback const & filterCallback = {},
      ScTemplateSearchResultCheckCallback const & checkCallback = {}) const;
  void Search(
      ScMemoryContext & ctx,
      ScTemplateSearchResultCallbackWithRequest const & callback,
      ScTemplateSearchResultFilterCallback const & filterCallback = {},
      ScTemplateSearchResultCheckCallback const & checkCallback = {}) const;

  SC_DEPRECATED(
      0.8.0,
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
  ScTemplateItemsToReplacementsItemsPositions m_templateItemsNamesToReplacementItemsPositions;
  // Store triples
  ScTemplateTriplesVector m_templateTriples;

  using ScTemplateGroupedTriples = std::unordered_set<size_t>;
  std::vector<ScTemplateGroupedTriples> m_priorityOrderedTemplateTriples;
  std::map<std::string, ScAddr> m_templateItemsNamesToReplacementItemsAddrs;
  std::map<std::string, ScType> m_templateItemsNamesToTypes;

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
    auto const & it = m_templateItemsNamesToReplacementItemsPositions.find(name);
    if (it != m_templateItemsNamesToReplacementItemsPositions.cend())
    {
      outAddr = m_replacementConstruction[it->second];
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
    auto const & it = m_templateItemsNamesToReplacementItemsPositions.find(name);
    if (it != m_templateItemsNamesToReplacementItemsPositions.cend())
    {
      if (it->second < Size())
      {
        return m_replacementConstruction[it->second];
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
      outAddr = m_replacementConstruction[index];
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
      return m_replacementConstruction[index];
    }

    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Index=" + std::to_string(index) + " must be < size=" + std::to_string(Size()));
  }

  SC_DEPRECATED(0.3.0, "Use ScTemplateGenResult::Size instead")
  inline size_t GetSize() const
  {
    return m_replacementConstruction.size();
  }

  //! Gets generated construction size
  inline size_t Size() const
  {
    return m_replacementConstruction.size();
  }

  inline ScTemplate::ScTemplateItemsToReplacementsItemsPositions const & GetReplacements() const
  {
    return m_templateItemsNamesToReplacementItemsPositions;
  }

protected:
  ScAddrVector m_replacementConstruction;

  ScTemplate::ScTemplateItemsToReplacementsItemsPositions m_templateItemsNamesToReplacementItemsPositions;
};

class ScTemplateSearchResultItem
{
  friend class ScTemplateSearch;
  friend class ScTemplateSearchResult;

public:
  ScTemplateSearchResultItem()
    : m_replacementConstruction(nullptr)
    , m_templateItemsNamesToReplacementItemPositions(nullptr)
  {
  }

  ScTemplateSearchResultItem(
      ScAddrVector const * results,
      ScTemplate::ScTemplateItemsToReplacementsItemsPositions const * replacements)
    : m_replacementConstruction(results)
    , m_templateItemsNamesToReplacementItemPositions(replacements)
  {
  }

  /* Gets found sc-element address by replacement `name`.
   * @param name A replacement name of sc-element address
   * @param outAddr[out] A found sc-element address by replacement `name`
   * @returns true, if sc-element address found by replacement `name`, otherwise false
   */
  inline bool Get(std::string const & name, ScAddr & outAddr) const noexcept
  {
    auto const & it = m_templateItemsNamesToReplacementItemPositions->find(name);
    if (it != m_templateItemsNamesToReplacementItemPositions->cend())
    {
      outAddr = (*m_replacementConstruction)[it->second];
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
    auto const & it = m_templateItemsNamesToReplacementItemPositions->find(name);
    if (it != m_templateItemsNamesToReplacementItemPositions->cend())
    {
      return (*m_replacementConstruction)[it->second];
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
      outAddr = (*m_replacementConstruction)[index];
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
      return (*m_replacementConstruction)[index];
    }

    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Index=" + std::to_string(index) + " must be < size=" + std::to_string(Size()));
  }

  //! Checks if replacement `name` exists in replacements
  inline bool Has(std::string const & name) const
  {
    return m_templateItemsNamesToReplacementItemPositions != nullptr &&
           m_templateItemsNamesToReplacementItemPositions->find(name) !=
               m_templateItemsNamesToReplacementItemPositions->cend();
  }

  //! Gets found construction size
  inline size_t Size() const
  {
    return m_replacementConstruction ? m_replacementConstruction->size() : 0;
  }

protected:
  ScAddrVector const * m_replacementConstruction;
  ScTemplate::ScTemplateItemsToReplacementsItemsPositions const * m_templateItemsNamesToReplacementItemPositions;
};

class ScTemplateSearchResult
{
  SC_DEPRECATED(
      0.8.0,
      "Use ScMemoryContext::HelperSearchTemplate(ScTemplate const & templ, ScTemplateSearchResultCallback const & "
      "callback, ScTemplateSearchResultCheckCallback const & checkCallback) instead.")

  friend class ScTemplateSearch;

public:
  inline size_t Size() const
  {
    return m_replacementConstructions.size();
  }

  inline bool IsEmpty() const
  {
    return Size() == 0;
  }

  SC_DEPRECATED(0.8.0, "Use ScTemplateSearchResult::Get(size_t index, ScTemplateSearchResultItem & outItem)")
  inline bool GetResultItemSafe(size_t index, ScTemplateSearchResultItem & outItem) const
  {
    return Get(index, outItem);
  }

  inline bool Get(size_t index, ScTemplateSearchResultItem & outItem) const
  {
    if (index < Size())
    {
      outItem.m_templateItemsNamesToReplacementItemPositions = &m_templateItemsNamesToReplacementItemsPositions;
      outItem.m_replacementConstruction = &(m_replacementConstructions[index]);
      return true;
    }

    return false;
  }

  inline ScTemplateSearchResultItem operator[](size_t index) const
  {
    if (index < Size())
    {
      return {&(m_replacementConstructions[index]), &m_templateItemsNamesToReplacementItemsPositions};
    }

    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Index=" + std::to_string(index) + " must be < size=" + std::to_string(Size()));
  }

  inline void Clear()
  {
    m_replacementConstructions.clear();
    m_templateItemsNamesToReplacementItemsPositions.clear();
  }

  inline ScTemplate::ScTemplateItemsToReplacementsItemsPositions const & GetReplacements() const
  {
    return m_templateItemsNamesToReplacementItemsPositions;
  }

  template <typename FnT>
  void ForEach(FnT && f)
  {
    for (auto const & res : m_replacementConstructions)
      f(ScTemplateSearchResultItem(&res, &m_templateItemsNamesToReplacementItemsPositions));
  }

protected:
  using SearchResults = std::vector<ScAddrVector>;
  SearchResults m_replacementConstructions;
  ScTemplate::ScTemplateItemsToReplacementsItemsPositions m_templateItemsNamesToReplacementItemsPositions;
};
