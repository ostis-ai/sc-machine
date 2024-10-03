/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <functional>

#include "sc_addr.hpp"
#include "sc_type.hpp"

#include "sc_utils.hpp"

/*!
 * @brief Represents an item in object of `ScTemplate`.
 *
 * ScTemplateItem is used to define elements within a sc-template, which can be a sc-address, type, or a replacement.
 */
struct _SC_EXTERN ScTemplateItem
{
  /*!
   * @brief Enum class for specifying the type of item in object of `ScTemplate`.
   */
  enum class Type : uint8_t
  {
    Type,    ///< The item is a type.
    Addr,    ///< The item is an address.
    Replace  ///< The item is a replacement.
  };

  /*!
   * @brief Default constructor. Initializes the item as a type.
   */
  _SC_EXTERN ScTemplateItem();

  /*!
   * @brief Generates a template item with a specified address.
   *
   * @param addr A sc-address.
   * @param replName Optional replacement name.
   */
  _SC_EXTERN ScTemplateItem(ScAddr const & addr, sc_char const * replName = nullptr);

  /*!
   * @brief Generates a template item with a specified type.
   *
   * @param type A sc-type.
   * @param replName Optional replacement name.
   */
  _SC_EXTERN ScTemplateItem(ScType const & type, sc_char const * replName = nullptr);

  /*!
   * @brief Generates a template item with a specified name.
   *
   * @param name A name of the item.
   */
  _SC_EXTERN ScTemplateItem(sc_char const * name);

  /*!
   * @brief Generates a template item with a specified name.
   *
   * @param name A name of the item.
   */
  _SC_EXTERN ScTemplateItem(std::string const & name);

  /*!
   * @brief Checks if the item is an address.
   *
   * @return true if the item is an address, false otherwise.
   */
  [[nodiscard]] _SC_EXTERN bool IsAddr() const;

  /*!
   * @brief Checks if the item is a replacement.
   *
   * @return true if the item is a replacement, false otherwise.
   */
  [[nodiscard]] _SC_EXTERN bool IsReplacement() const;

  /*!
   * @brief Checks if the item is a type.
   *
   * @return true if the item is a type, false otherwise.
   */
  [[nodiscard]] _SC_EXTERN bool IsType() const;

  /*!
   * @brief Checks if the item is fixed (either an address or a valid replacement).
   *
   * @return true if the item is fixed, false otherwise.
   */
  [[nodiscard]] _SC_EXTERN bool IsFixed() const;

  /*!
   * @brief Checks if the item is assignable.
   *
   * @return true if the item is assignable, false otherwise.
   */
  [[nodiscard]] _SC_EXTERN bool IsAssign() const;

  /*!
   * @brief Checks if the item has a name.
   *
   * @return true if the item has a name, false otherwise.
   */
  [[nodiscard]] _SC_EXTERN bool HasName() const;

  /*!
   * @brief Gets a pretty name for the item, enclosed in backticks.
   *
   * @return A string representing the pretty name.
   */
  [[nodiscard]] _SC_EXTERN std::string GetPrettyName() const;

  /*!
   * @brief Sets the item as an address.
   *
   * @param addr A sc-address.
   * @param replName Optional replacement name.
   */
  _SC_EXTERN void SetAddr(ScAddr const & addr, sc_char const * replName = nullptr);

  /*!
   * @brief Sets the item as a type.
   *
   * @param type A sc-type.
   * @param replName Optional replacement name.
   */
  _SC_EXTERN void SetType(ScType const & type, sc_char const * replName = nullptr);

  /*!
   * @brief Sets the item as a replacement.
   *
   * @param name A replacement name.
   */
  _SC_EXTERN void SetReplacement(sc_char const * name);

  Type m_itemType = Type::Type;          ///< A type of the item.
  ScAddr m_addrValue;                    ///< A value sc-address of the item.
  ScType m_typeValue = ScType::Unknown;  ///< A type value of the item.
  std::string m_name;                    ///< A name of the item.
};

/*!
 * @brief Overloaded operator for creating a ScTemplateItem from an address with a replacement name.
 *
 * @param value A sc-address.
 * @param replName A replacement name.
 * @return A ScTemplateItem initialized with the address and replacement name.
 */
_SC_EXTERN ScTemplateItem operator>>(ScAddr const & value, sc_char const * replName);

/*!
 * @brief Overloaded operator for creating a ScTemplateItem from an address with a replacement name.
 *
 * @param value A sc-address.
 * @param replName A replacement name as a string.
 * @return A ScTemplateItem initialized with the address and replacement name.
 */
_SC_EXTERN ScTemplateItem operator>>(ScAddr const & value, std::string const & replName);

/*!
 * @brief Overloaded operator for creating a ScTemplateItem from a type with a replacement name.
 *
 * @param value A sc-type.
 * @param replName A replacement name.
 * @return A ScTemplateItem initialized with the type and replacement name.
 */
_SC_EXTERN ScTemplateItem operator>>(ScType const & value, sc_char const * replName);

/*!
 * @brief Overloaded operator for creating a ScTemplateItem from a type with a replacement name.
 *
 * @param value A sc-type.
 * @param replName A replacement name as a string.
 * @return A ScTemplateItem initialized with the type and replacement name.
 */
_SC_EXTERN ScTemplateItem operator>>(ScType const & value, std::string const & replName);

class ScTemplateResultItem;
class ScTemplateSearchResult;

enum class _SC_EXTERN ScTemplateResultCode : uint8_t
{
  Success = 0,
  InvalidParams = 1,
  InternalError = 2
};

/*!
 * @brief Parameters for template generator and searcher.
 *
 * ScTemplateParams can be used to replace variables by values in a sc-template.
 */
class _SC_EXTERN ScTemplateParams
{
  friend class ScTemplateGenerator;

public:
  using ScTemplateItemsToParams = std::map<std::string, ScAddr>;

  /*!
   * @brief Default constructor.
   */
  _SC_EXTERN ScTemplateParams() = default;

  /*!
   * @brief Adds a variable identifier and its corresponding value.
   *
   * @param varIdtf A variable identifier.
   * @param value A sc-address of variable value.
   * @return A reference to the current ScTemplateParams object.
   * @throws utils::ExceptionInvalidParams if the variable identifier already has a value.
   */
  _SC_EXTERN ScTemplateParams & Add(std::string const & varIdtf, ScAddr const & value) noexcept(false);

  /*!
   * @brief Adds a variable address and its corresponding value.
   *
   * @param varAddr A variable sc-address.
   * @param value A sc-address of variable value.
   * @return A reference to the current ScTemplateParams object.
   * @throws utils::ExceptionInvalidParams if the variable address already has a value.
   */
  _SC_EXTERN ScTemplateParams & Add(ScAddr const & varAddr, ScAddr const & value) noexcept(false);

  /*!
   * @brief Gets the value associated with a variable identifier.
   *
   * @param varIdtf A variable identifier.
   * @param outAddr A sc-address of variable value.
   * @return true if the value is found, false otherwise.
   */
  _SC_EXTERN bool Get(std::string const & varIdtf, ScAddr & outAddr) const noexcept;

  /*!
   * @brief Gets the value associated with a variable address.
   *
   * @param varAddr The variable sc-address.
   * @param outAddr A sc-address of variable value.
   * @return true if the value is found, false otherwise.
   */
  _SC_EXTERN bool Get(ScAddr const & varAddr, ScAddr & outAddr) const noexcept;

  /*!
   * @brief Gets all items in object of `ScTemplate` and their corresponding parameters.
   *
   * @return A map of template items to parameters.
   */
  _SC_EXTERN ScTemplateItemsToParams GetAll() const;

  /*!
   * @brief Checks if the parameters are empty.
   *
   * @return true if the parameters are empty, false otherwise.
   */
  [[nodiscard]] _SC_EXTERN bool IsEmpty() const noexcept;

  /*!
   * @brief An empty ScTemplateParams object.
   */
  _SC_EXTERN static const ScTemplateParams Empty;

protected:
  ScTemplateItemsToParams m_templateItemsToParams;  ///< Map of template items to parameters.
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

/*!
 * @brief Represents a program object of sc-template used for generating and searching sc-elements in sc-memory.
 *
 * ScTemplate allows the creation of templates for sc-elements, which can be used to generate or search for specific
 * patterns in sc-memory.
 */
class _SC_EXTERN ScTemplate
{
  friend class ScMemoryContext;
  friend class ScTemplateSearch;
  friend class ScTemplateGenerator;
  friend class ScTemplateBuilder;
  friend class ScTemplateBuilderFromScs;
  friend class ScTemplateLoader;

public:
  /*!
   * @brief Represents the result of a sc-template operation.
   */
  class _SC_EXTERN Result
  {
  public:
    /*!
     * @brief Generates a Result object.
     *
     * @param result The result of the operation (true for success, false for failure).
     * @param errorMsg Optional error message.
     */
    _SC_EXTERN explicit Result(bool result, std::string errorMsg = "");

    /*!
     * @brief Conversion operator to bool.
     *
     * @return true if the result is successful, false otherwise.
     */
    _SC_EXTERN operator bool() const;

    /*!
     * @brief Gets the error message.
     *
     * @return A string representing the error message.
     */
    [[nodiscard]] _SC_EXTERN std::string const & Msg() const;

  private:
    bool m_result = false;  ///< A result of the operation.
    std::string m_msg;      ///< An error message.
  };

public:
  SC_DISALLOW_COPY(ScTemplate);
  ScTemplate(ScTemplate && other) noexcept;
  ScTemplate & operator=(ScTemplate && other) noexcept;

  using ScTemplateItemsToReplacementsItemsPositions = std::unordered_map<std::string, size_t>;
  using ScTemplateTriplesVector = std::vector<class ScTemplateTriple *>;

  _SC_EXTERN explicit ScTemplate() noexcept;

  _SC_EXTERN ~ScTemplate() noexcept;

  /*!
   * @brief Adds a triple to object of `ScTemplate`.
   *
   * @param param1 The first item in the triple.
   * @param param2 The second item in the triple.
   * @param param3 The third item in the triple.
   * @return A reference to the current ScTemplate object.
   * @throws utils::ExceptionInvalidParams if the parameters are invalid.
   */
  _SC_EXTERN ScTemplate & operator()(
      ScTemplateItem const & param1,
      ScTemplateItem const & param2,
      ScTemplateItem const & param3) noexcept(false);

  /*!
   * @brief Adds a quintuple to object of `ScTemplate`.
   *
   * @param param1 The first item in the quintuple.
   * @param param2 The second item in the quintuple.
   * @param param3 The third item in the quintuple.
   * @param param4 The fourth item in the quintuple.
   * @param param5 The fifth item in the quintuple.
   * @return A reference to the current ScTemplate object.
   * @throws utils::ExceptionInvalidParams if the parameters are invalid.
   */
  _SC_EXTERN ScTemplate & operator()(
      ScTemplateItem const & param1,
      ScTemplateItem const & param2,
      ScTemplateItem const & param3,
      ScTemplateItem const & param4,
      ScTemplateItem const & param5) noexcept(false);

  /*!
   * @brief Removes all constructions from object of `ScTemplate`.
   */
  _SC_EXTERN void Clear();

  /*!
   * @brief Checks if object of `ScTemplate` is empty.
   *
   * @return true if object of `ScTemplate` is empty, false otherwise.
   */
  [[nodiscard]] _SC_EXTERN bool IsEmpty() const;

  /*!
   * @brief Gets size of object of `ScTemplate`.
   *
   * @return The number of triples in object of `ScTemplate`.
   */
  [[nodiscard]] _SC_EXTERN size_t Size() const;

  /*!
   * @brief Checks if object of `ScTemplate` has a replacement for a given name.
   *
   * @param repl The replacement name.
   * @return true if the replacement exists, false otherwise.
   */
  _SC_EXTERN bool HasReplacement(std::string const & repl) const;

  /*!
   * @brief Checks if object of `ScTemplate` has a replacement for a given sc-address of variable.
   *
   * @param replAddr The replacement address.
   * @return true if the replacement exists, false otherwise.
   */
  _SC_EXTERN bool HasReplacement(ScAddr const & replAddr) const;

  /*!
   * @brief Adds a triple to object of `ScTemplate`.
   *
   * @param param1 The first item in the triple.
   * @param param2 The second item in the triple.
   * @param param3 The third item in the triple.
   *
   *          param2
   * param1 ----------> param3
   *
   * @return A reference to the current ScTemplate object.
   * @throws utils::ExceptionInvalidParams if the parameters are invalid.
   */
  _SC_EXTERN ScTemplate & Triple(
      ScTemplateItem const & param1,
      ScTemplateItem const & param2,
      ScTemplateItem const & param3) noexcept(false);

  /*!
   * @brief Adds a quintuple to object of `ScTemplate`.
   *
   * @param param1 The first item in the quintuple.
   * @param param2 The second item in the quintuple.
   * @param param3 The third item in the quintuple.
   * @param param4 The fourth item in the quintuple.
   * @param param5 The fifth item in the quintuple.
   *
   *           param2
   *	param1 ---------> param3
   *             ^
   *             |
   *             | param4
   *             |
   *           param5
   * Equal to two calls of triple, so this template add 6 items to result (NOT 5), to minimize
   * possible abuse, use result name mapping, and get result by names.
   *
   * @return A reference to the current ScTemplate object.
   * @throws utils::ExceptionInvalidParams if the parameters are invalid.
   */
  _SC_EXTERN ScTemplate & Quintuple(
      ScTemplateItem const & param1,
      ScTemplateItem const & param2,
      ScTemplateItem const & param3,
      ScTemplateItem const & param4,
      ScTemplateItem const & param5) noexcept(false);

protected:
  // Begin: calls by memory context

  /*!
   * @brief Generates sc-elements based by object of `ScTemplate`.
   *
   * @param context A sc-memory context.
   * @param result A result item to store generated elements.
   * @param params A template parameters.
   * @param errorCode Optional error code.
   * @return A result of the generation.
   * @throws utils::ExceptionInvalidParams if the parameters are invalid.
   */
  Result Generate(
      ScMemoryContext & context,
      ScTemplateResultItem & result,
      ScTemplateParams const & params,
      ScTemplateResultCode * errorCode = nullptr) const noexcept(false);

  /*!
   * @brief Searches for sc-elements by object of `ScTemplate`.
   *
   * @param context A sc-memory context.
   * @param result A result item to store the found elements.
   * @return A result of the search.
   * @throws utils::ExceptionInvalidParams if the parameters are invalid.
   */
  Result Search(ScMemoryContext & context, ScTemplateSearchResult & result) const noexcept(false);

  /*!
   * @brief Searches for sc-elements by object of `ScTemplate` with callbacks.
   *
   * @param context A sc-memory context.
   * @param callback A callback to handle the search results.
   * @param filterCallback Optional filter callback.
   * @param checkCallback Optional check callback.
   * @throws utils::ExceptionInvalidParams if the parameters are invalid.
   */
  void Search(
      ScMemoryContext & context,
      ScTemplateSearchResultCallback const & callback,
      ScTemplateSearchResultFilterCallback const & filterCallback = {},
      ScTemplateSearchResultCheckCallback const & checkCallback = {}) const noexcept(false);

  /*!
   * @brief Searches for sc-elements by object of `ScTemplate` with request callbacks.
   *
   * @param context A sc-memory context.
   * @param callback A callback to handle the search results with requests.
   * @param filterCallback Optional filter callback.
   * @param checkCallback Optional check callback.
   * @throws utils::ExceptionInvalidParams if the parameters are invalid.
   */
  void Search(
      ScMemoryContext & context,
      ScTemplateSearchResultCallbackWithRequest const & callback,
      ScTemplateSearchResultFilterCallback const & filterCallback = {},
      ScTemplateSearchResultCheckCallback const & checkCallback = {}) const noexcept(false);

  /*!
   * @brief Translates a sc-template in sc-memory (sc-structure) into object of `ScTemplate`.
   *
   * @param context A sc-memory context.
   * @param translatableTemplateAddr A sc-address of sc-template in sc-memory to be translated in object of
   * `ScTemplate`.
   * @param params Optional sc-template parameters.
   * @throws utils::ExceptionInvalidParams if the parameters are invalid.
   */
  void TranslateFrom(
      ScMemoryContext & context,
      ScAddr const & translatableTemplateAddr,
      ScTemplateParams const & params = ScTemplateParams()) noexcept(false);

  /*!
   * @brief Translates a sc-template represented in SCs-code into object of `ScTemplate`.
   *
   * @param context A sc-memory context.
   * @param translatableSCsTemplate A sc.s-template to be translated to object of `ScTemplate`.
   * @throws utils::ExceptionInvalidParams if the parameters are invalid.
   */
  void TranslateFrom(ScMemoryContext & context, std::string const & translatableSCsTemplate) noexcept(false);

  /*!
   * @brief Translates an object of `ScTemplate` to a sc-template in sc-memory (sc-structure).
   *
   * @param context A sc-memory context.
   * @param resultTemplateAddr A sc-address of sc-template to be gotten in sc-memory.
   * @param params Optional sc-template parameters.
   * @throws utils::ExceptionInvalidParams if the parameters are invalid.
   */
  void TranslateTo(
      ScMemoryContext & context,
      ScAddr & resultTemplateAddr,
      ScTemplateParams const & params = ScTemplateParams()) noexcept(false);

protected:
  // Store mapping of name to index in result vector
  ScTemplateItemsToReplacementsItemsPositions
      m_templateItemsNamesToReplacementItemsPositions;  ///< Map of template items to replacement items positions.
  // Store triples
  ScTemplateTriplesVector m_templateTriples;  ///< Vector of template triples.
  using ScTemplateGroupedTriples = std::unordered_set<size_t>;
  std::vector<ScTemplateGroupedTriples>
      m_priorityOrderedTemplateTriples;  ///< Vector of priority ordered template triples.
  std::map<std::string, ScAddr>
      m_templateItemsNamesToReplacementItemsAddrs;  ///< Map of template items names to replacement items addresses.
  std::map<std::string, ScType> m_templateItemsNamesToTypes;  ///< Map of template items names to types.

  enum class ScTemplateTripleType : uint8_t
  {
    AFA = 0,  // _... -> _...
    FAF = 1,  // ... _-> ...
    AAF = 2,  // _... _-> ...
    FAN = 3,  // ... _-> ...
    FAE = 4,  // ... _-> _connector
    AAA = 5,  // _... _-> _...

    ScConstr3TypeCount
  };

  /*!
   * @brief Gets the priority of a template triple.
   *
   * @param triple The template triple.
   * return ScTemplateTripleType representing triple priority.
   */
  ScTemplateTripleType GetPriority(ScTemplateTriple * triple);
};

/*!
 * @brief Represents an item in the result of a sc-template operation.
 *
 * ScTemplateResultItem is used to store and manage the results of sc-template operations, providing access to the found
 * sc-elements.
 */
class _SC_EXTERN ScTemplateResultItem
{
  friend class ScTemplateGenerator;
  friend class ScSet;
  friend class ScTemplateSearch;
  friend class ScTemplateSearchResult;

public:
  _SC_EXTERN ScTemplateResultItem();

  _SC_EXTERN ~ScTemplateResultItem();

  _SC_EXTERN ScTemplateResultItem(ScTemplateResultItem const & otherItem);

  _SC_EXTERN ScTemplateResultItem & operator=(ScTemplateResultItem const & otherItem);

  /*! Gets found sc-element address by `varAddr`.
   * @param varAddr A template var sc-element address
   * @param outAddr[out] A found sc-element address by `varAddr`
   * @returns true, if sc-element address found by `varAddr`, otherwise false
   */
  _SC_EXTERN bool Get(ScAddr const & varAddr, ScAddr & outAddr) const noexcept;

  /*! Gets found sc-element address by `varAddr`.
   * @param varAddr A template var sc-element address
   * @returns Found sc-element address.
   * @throws utils::ExceptionInvalidParams if `varAddr` is invalid or not found in replacements.
   */
  _SC_EXTERN ScAddr operator[](ScAddr const & varAddr) const noexcept(false);

  /*! Gets found sc-element address by replacement `name`.
   * @param name A replacement name of sc-element address
   * @param outAddr[out] A found sc-element address by replacement `name`
   * @returns true, if sc-element address found by replacement `name`, otherwise false
   */
  _SC_EXTERN bool Get(std::string const & name, ScAddr & outAddr) const noexcept;

  /*! Gets found sc-element address by replacement `name`.
   * @param name A replacement name of sc-element address
   * @returns Found sc-element address.
   * @throws utils::ExceptionInvalidParams if `name` is invalid or not found in replacements.
   */
  _SC_EXTERN ScAddr operator[](std::string const & name) const noexcept(false);

  /*! Gets found sc-element address by `index` in triple.
   * @param name A replacement name of sc-element address
   * @param outAddr[out] A found sc-element address by `index` in triple
   * @returns true, if sc-element address found by `index` in triple, otherwise false
   */
  _SC_EXTERN bool Get(size_t index, ScAddr & outAddr) const noexcept;

  /*! Gets found sc-element address by `index` in triple.
   * @param name A replacement name of sc-element address
   * @returns Found sc-element address.
   * @throws utils::ExceptionInvalidParams if `index` < found construction size.
   */
  _SC_EXTERN ScAddr const & operator[](size_t index) const noexcept(false);

  /*!
   * @brief Checks if a replacement name exists in the replacements.
   *
   * @param name A replacement name.
   * @return true if the replacement name exists, false otherwise.
   */
  _SC_EXTERN bool Has(std::string const & name) const noexcept;

  /*!
   * @brief Checks if a variable address exists in the replacements.
   *
   * @param varAddr A variable sc-element address.
   * @return true if the variable address exists, false otherwise.
   */
  _SC_EXTERN bool Has(ScAddr const & varAddr) const noexcept;

  /*!
   * @brief Gets the size of the found construction.
   *
   * @return Size of the found construction.
   */
  _SC_EXTERN size_t Size() const noexcept;

  /*!
   * @brief Gets an iterator to the beginning of the found construction.
   *
   * @return A constant iterator to the beginning of the found construction.
   */
  _SC_EXTERN ScAddrVector::const_iterator begin() const;

  /*!
   * @brief Gets an iterator to the end of the found construction.
   *
   * @return A constant iterator to the end of the found construction.
   */
  _SC_EXTERN ScAddrVector::const_iterator end() const;

  /*!
   * @brief Gets the map of template items to replacement item positions.
   *
   * @return The map of template items to replacement item positions.
   */
  _SC_EXTERN ScTemplate::ScTemplateItemsToReplacementsItemsPositions const & GetReplacements() const noexcept;

protected:
  ScTemplateResultItem(
      ScMemoryContext * context,
      ScAddrVector results,
      ScTemplate::ScTemplateItemsToReplacementsItemsPositions replacements);
  _SC_EXTERN ScTemplateResultItem(
      ScMemoryContext * context,
      ScTemplate::ScTemplateItemsToReplacementsItemsPositions replacements);

  ScAddr GetAddrByName(std::string const & name) const;

  ScAddr GetAddrByVarAddr(ScAddr const & varAddr) const;

  ScMemoryContext * m_context;

  ScAddrVector m_replacementConstruction;  ///< A vector of sc-addresses that are results.
  ScTemplate::ScTemplateItemsToReplacementsItemsPositions
      m_templateItemsNamesToReplacementItemPositions;  ///< A map of template items to replacement item positions.
};

using ScTemplateGenResult = ScTemplateResultItem;
using ScTemplateSearchResultItem = ScTemplateResultItem;

/**
 * @brief Represents the result of a sc-template search operation.
 *
 * ScTemplateSearchResult is used to store and manage the results of sc-template search operations, providing access to
 * the found sc-elements.
 */
class _SC_EXTERN ScTemplateSearchResult
{
  friend class ScTemplateSearch;

public:
  _SC_EXTERN ScTemplateSearchResult() noexcept;

  /**
   * @brief Gets the number of found constructions.
   *
   * @return The number of found constructions.
   */
  [[nodiscard]] _SC_EXTERN size_t Size() const noexcept;

  /**
   * @brief Checks if the search result is empty.
   *
   * @return true if the search result is empty, false otherwise.
   */
  [[nodiscard]] _SC_EXTERN bool IsEmpty() const noexcept;

  /**
   * @brief Gets result item at the specified index.
   *
   * @param index An index of the result item.
   * @param outItem [out] A result item at the specified index.
   * @return true if the result item is found, false otherwise.
   */
  _SC_EXTERN bool Get(size_t index, ScTemplateResultItem & outItem) const noexcept;

  /**
   * @brief Gets result item at the specified index.
   *
   * @param index An index of the result item.
   * @return A result item at the specified index.
   * @throws utils::ExceptionInvalidParams if the index is invalid or out of range.
   */
  _SC_EXTERN ScTemplateResultItem operator[](size_t index) const noexcept(false);

  /**
   * @brief Clears search results.
   */
  _SC_EXTERN void Clear() noexcept;

  /**
   * @brief Gets the map of template items to replacement item positions.
   *
   * @return A map of template items to replacement item positions.
   */
  _SC_EXTERN ScTemplate::ScTemplateItemsToReplacementsItemsPositions GetReplacements() const noexcept;

  /**
   * @brief Iterates over each search result item and applies the provided function.
   *
   * @tparam FnT A type of the function to apply.
   * @param f A function to apply to each search result item.
   */
  template <typename FnT>
  _SC_EXTERN void ForEach(FnT && f) noexcept
  {
    for (auto & res : m_replacementConstructions)
      f(ScTemplateResultItem{m_context, res, m_templateItemsNamesToReplacementItemsPositions});
  }

protected:
  ScMemoryContext * m_context = nullptr;
  using SearchResults = std::vector<ScAddrVector>;
  SearchResults m_replacementConstructions;  ///< A vector of replacement constructions.
  ScTemplate::ScTemplateItemsToReplacementsItemsPositions
      m_templateItemsNamesToReplacementItemsPositions;  ///< A map of template items to replacement item positions.
};
