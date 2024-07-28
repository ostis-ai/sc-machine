/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_addr.hpp"
#include "sc_type.hpp"

#include "sc_utils.hpp"

struct _SC_EXTERN ScTemplateItem
{
  enum class Type : uint8_t
  {
    Type,
    Addr,
    Replace
  };

  _SC_EXTERN ScTemplateItem();

  _SC_EXTERN ScTemplateItem(ScAddr const & addr, sc_char const * replName = nullptr);

  _SC_EXTERN ScTemplateItem(ScType const & type, sc_char const * replName = nullptr);

  _SC_EXTERN ScTemplateItem(sc_char const * name);

  _SC_EXTERN ScTemplateItem(std::string const & name);

  [[nodiscard]] _SC_EXTERN bool IsAddr() const;

  [[nodiscard]] _SC_EXTERN bool IsReplacement() const;

  [[nodiscard]] _SC_EXTERN bool IsType() const;

  [[nodiscard]] _SC_EXTERN bool IsFixed() const;

  [[nodiscard]] _SC_EXTERN bool IsAssign() const;

  [[nodiscard]] _SC_EXTERN bool HasName() const;

  [[nodiscard]] _SC_EXTERN std::string GetPrettyName() const;

  _SC_EXTERN void SetAddr(ScAddr const & addr, sc_char const * replName = nullptr);

  _SC_EXTERN void SetType(ScType const & type, sc_char const * replName = nullptr);

  _SC_EXTERN void SetReplacement(sc_char const * name);

  Type m_itemType = Type::Type;

  ScAddr m_addrValue;
  ScType m_typeValue;
  std::string m_name;
};

_SC_EXTERN ScTemplateItem operator>>(ScAddr const & value, sc_char const * replName);
_SC_EXTERN ScTemplateItem operator>>(ScAddr const & value, std::string const & replName);
_SC_EXTERN ScTemplateItem operator>>(ScType const & value, sc_char const * replName);
_SC_EXTERN ScTemplateItem operator>>(ScType const & value, std::string const & replName);

class ScTemplateResultItem;
class ScTemplateSearchResult;

enum class _SC_EXTERN ScTemplateResultCode : uint8_t
{
  Success = 0,
  InvalidParams = 1,
  InternalError = 2
};

/* Parameters for template generator.
 * Can be used to replace variables by values
 */
class _SC_EXTERN ScTemplateParams
{
  friend class ScTemplateGenerator;

public:
  using ScTemplateItemsToParams = std::map<std::string, ScAddr>;

  _SC_EXTERN ScTemplateParams() = default;

  _SC_EXTERN ScTemplateParams & Add(std::string const & varIdtf, ScAddr const & value) noexcept(false);

  _SC_EXTERN ScTemplateParams & Add(ScAddr const & varAddr, ScAddr const & value) noexcept(false);

  _SC_EXTERN bool Get(std::string const & varIdtf, ScAddr & outAddr) const noexcept;

  _SC_EXTERN bool Get(ScAddr const & varAddr, ScAddr & outAddr) const noexcept;

  _SC_EXTERN ScTemplateItemsToParams GetAll() const;

  [[nodiscard]] _SC_EXTERN bool IsEmpty() const noexcept;

  _SC_EXTERN static const ScTemplateParams Empty;

protected:
  ScTemplateItemsToParams m_templateItemsToParams;
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

class _SC_EXTERN ScTemplate final
{
  friend class ScMemoryContext;
  friend class ScTemplateSearch;
  friend class ScTemplateGenerator;
  friend class ScTemplateBuilder;
  friend class ScTemplateBuilderFromScs;

public:
  class _SC_EXTERN Result
  {
  public:
    _SC_EXTERN explicit Result(bool result, std::string errorMsg = "");

    _SC_EXTERN operator bool() const;

    [[nodiscard]] _SC_EXTERN std::string const & Msg() const;

  private:
    bool m_result = false;
    std::string m_msg;
  };

public:
  SC_DISALLOW_COPY_AND_MOVE(ScTemplate);

  using ScTemplateItemsToReplacementsItemsPositions = std::unordered_map<std::string, size_t>;
  using ScTemplateTriplesVector = std::vector<class ScTemplateTriple *>;

  _SC_EXTERN explicit ScTemplate();

  _SC_EXTERN ~ScTemplate();

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

  _SC_EXTERN size_t Size() const;

  _SC_EXTERN bool HasReplacement(std::string const & repl) const;

  _SC_EXTERN bool HasReplacement(ScAddr const & replAddr) const;

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

protected:
  // Begin: calls by memory context
  Result Generate(
      ScMemoryContext & ctx,
      ScTemplateResultItem & result,
      ScTemplateParams const & params,
      ScTemplateResultCode * errorCode = nullptr) const noexcept(false);

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

  // Builds template based on template in sc-memory
  Result FromScTemplate(
      ScMemoryContext & ctx,
      ScAddr const & scTemplateAddr,
      ScTemplateParams const & params = ScTemplateParams()) noexcept(false);
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

  ScTemplateTripleType GetPriority(ScTemplateTriple * triple);
};

class _SC_EXTERN ScTemplateResultItem
{
  friend class ScTemplateGenerator;
  friend class ScSet;
  friend class ScTemplateSearch;
  friend class ScTemplateSearchResult;

public:
  _SC_EXTERN ScTemplateResultItem();

  _SC_EXTERN ~ScTemplateResultItem();

  _SC_EXTERN ScTemplateResultItem(
      ScMemoryContext * context,
      ScAddrVector results,
      ScTemplate::ScTemplateItemsToReplacementsItemsPositions replacements);
  _SC_EXTERN ScTemplateResultItem(ScMemoryContext * context, ScTemplate::ScTemplateItemsToReplacementsItemsPositions replacements);

  _SC_EXTERN ScTemplateResultItem(ScTemplateResultItem const & otherItem);

  _SC_EXTERN ScTemplateResultItem & operator=(ScTemplateResultItem const & otherItem);

  /* Gets found sc-element address by `varAddr`.
   * @param varAddr A template var sc-element address
   * @param outAddr[out] A found sc-element address by `varAddr`
   * @returns true, if sc-element address found by `varAddr`, otherwise false
   */
  _SC_EXTERN bool Get(ScAddr const & varAddr, ScAddr & outAddr) const noexcept;

  /* Gets found sc-element address by `varAddr`.
   * @param varAddr A template var sc-element address
   * @returns Found sc-element address.
   * @throws utils::ExceptionInvalidParams if `varAddr` is invalid or not found in replacements.
   */
  _SC_EXTERN ScAddr operator[](ScAddr const & varAddr) const noexcept(false);

  /* Gets found sc-element address by replacement `name`.
   * @param name A replacement name of sc-element address
   * @param outAddr[out] A found sc-element address by replacement `name`
   * @returns true, if sc-element address found by replacement `name`, otherwise false
   */
  _SC_EXTERN bool Get(std::string const & name, ScAddr & outAddr) const noexcept;

  /* Gets found sc-element address by replacement `name`.
   * @param name A replacement name of sc-element address
   * @returns Found sc-element address.
   * @throws utils::ExceptionInvalidParams if `name` is invalid or not found in replacements.
   */
  _SC_EXTERN ScAddr operator[](std::string const & name) const noexcept(false);

  /* Gets found sc-element address by `index` in triple.
   * @param name A replacement name of sc-element address
   * @param outAddr[out] A found sc-element address by `index` in triple
   * @returns true, if sc-element address found by `index` in triple, otherwise false
   */
  _SC_EXTERN bool Get(size_t index, ScAddr & outAddr) const noexcept;

  /* Gets found sc-element address by `index` in triple.
   * @param name A replacement name of sc-element address
   * @returns Found sc-element address.
   * @throws utils::ExceptionInvalidParams if `index` < found construction size.
   */
  _SC_EXTERN ScAddr const & operator[](size_t index) const noexcept(false);

  //! Checks if replacement `name` exists in replacements
  _SC_EXTERN bool Has(std::string const & name) const noexcept;

  //! Checks if `varAddr` exists in replacements
  _SC_EXTERN bool Has(ScAddr const & varAddr) const noexcept;

  //! Gets found construction size
  _SC_EXTERN size_t Size() const noexcept;

  _SC_EXTERN ScAddrVector::const_iterator begin() const;

  _SC_EXTERN ScAddrVector::const_iterator end() const;

  _SC_EXTERN ScTemplate::ScTemplateItemsToReplacementsItemsPositions const & GetReplacements() const noexcept;

protected:
  ScAddr GetAddrByName(std::string const & name) const;

  ScAddr GetAddrByVarAddr(ScAddr const & varAddr) const;

  ScMemoryContext * m_context;

  ScAddrVector m_replacementConstruction;
  ScTemplate::ScTemplateItemsToReplacementsItemsPositions m_templateItemsNamesToReplacementItemPositions;
};

using ScTemplateGenResult = ScTemplateResultItem;
using ScTemplateSearchResultItem = ScTemplateResultItem;

class _SC_EXTERN ScTemplateSearchResult
{
  friend class ScTemplateSearch;

public:
  _SC_EXTERN ScTemplateSearchResult() noexcept;

  _SC_EXTERN size_t Size() const noexcept;

  _SC_EXTERN bool IsEmpty() const noexcept;

  _SC_EXTERN bool Get(size_t index, ScTemplateResultItem & outItem) const noexcept;

  _SC_EXTERN ScTemplateResultItem operator[](size_t index) const noexcept(false);

  _SC_EXTERN void Clear() noexcept;

  _SC_EXTERN ScTemplate::ScTemplateItemsToReplacementsItemsPositions GetReplacements() const noexcept;

  template <typename FnT>
  _SC_EXTERN void ForEach(FnT && f) noexcept
  {
    for (auto & res : m_replacementConstructions)
      f(ScTemplateResultItem{m_context, res, m_templateItemsNamesToReplacementItemsPositions});
  }

protected:
  ScMemoryContext * m_context = nullptr;

  using SearchResults = std::vector<ScAddrVector>;
  SearchResults m_replacementConstructions;
  ScTemplate::ScTemplateItemsToReplacementsItemsPositions m_templateItemsNamesToReplacementItemsPositions;
};
