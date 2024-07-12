/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <utility>

#include "sc_type.hpp"
#include "sc_defines.hpp"

#include "sc_object.hpp"
#include "sc_addr.hpp"
#include "sc_memory.hpp"

namespace internal
{
/*!
 * @class A base class to register keynodes.
 * @brief It reminds keynodes data and resolve them after memory initialization.
 * @note This class is just for internal usage.
 */
class ScKeynodesRegister
{
public:
  /*!
   * @brief Remembers keynode data to register it after.
   * @param keynode Pointer to remembering keynode data.
   * @param idtf Keynode system identifier.
   * @param keynodeType Keynode syntactic type.
   */
  static void Remember(ScAddr * keynode, std::string_view const & idtf, ScType const & keynodeType)
  {
    m_keynodes.insert({keynode, {idtf, keynodeType}});
  }

  static void Forget(ScAddr * keynode)
  {
    m_keynodes.erase(keynode);
  }

  /*!
   * @brief Registers all reminded keynodes.
   * @param context Sc-memory context to resolve keynodes.
   */
  static void Register(ScMemoryContext * context, ScAddr initMemoryGeneratedStructure)
  {
    for (auto const & item : m_keynodes)
    {
      ScAddr * keynode = item.first;
      auto const & keynodeInfo = item.second;
      ScSystemIdentifierQuintuple fiver;
      context->HelperResolveSystemIdtf(std::string(keynodeInfo.first), keynodeInfo.second, fiver);
      *keynode = fiver.addr1;

      if (initMemoryGeneratedStructure.IsValid())
      {
        context->CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructure, fiver.addr1);
        context->CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructure, fiver.addr2);
        context->CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructure, fiver.addr3);
        context->CreateEdge(ScType::EdgeAccessConstPosPerm, initMemoryGeneratedStructure, fiver.addr4);
      }
    }
  }

  static void Unregister(ScMemoryContext *) {}

protected:
  static inline std::map<ScAddr *, std::pair<std::string_view, ScType>> m_keynodes;
};
}  // namespace internal

/*!
 * @class A base class for keynodes. Use it to create keynode in ScKeynodes class to use in agent programs.
 * @example
 * \code
 * static ScKeynode<ScType::NodeConst> const keynode{"keynode_const_node"};
 * \endcode
 * @warning Use it only for static objects declaration.
 */
template <ScType const & type>
class ScKeynode : public ScAddr
{
public:
  explicit ScKeynode(std::string_view const & sysIdtf)
    : ScAddr(ScAddr::Empty)
  {
    internal::ScKeynodesRegister::Remember(this, sysIdtf, type);
  }

  explicit ScKeynode(sc_addr const & addr)
    : ScAddr(addr)
  {
  }

  ~ScKeynode()
  {
    internal::ScKeynodesRegister::Forget(this);
  }

  ScKeynode(ScKeynode const & other)
    : ScAddr(other)
  {
  }

  ScKeynode & operator=(ScKeynode const &)
  {
    return *this;
  }
};

/*!
 * @class A base class for const class keynodes. Use it to create keynode in ScKeynodes class to use in agent programs.
 * @example
 * \code
 * static ScKeynodeClass const kActionClass{"action_class"};
 * \endcode
 * @warning Use it only for static objects declaration.
 */
class ScKeynodeClass : public ScKeynode<ScType::NodeConstClass>
{
public:
  explicit ScKeynodeClass()
    : ScKeynode(SC_ADDR_EMPTY)
  {
  }

  explicit ScKeynodeClass(std::string_view const & sysIdtf)
    : ScKeynode(sysIdtf)
  {
  }
};

/*!
 * @class A base class for const no role keynodes. Use it to create keynode in ScKeynodes class to use in agent
 * programs.
 * @example
 * \code
 * static ScKeynodeNoRole const kNrelExample{"nrel_example"};
 * \endcode
 * @warning Use it only for static objects declaration.
 */
class ScKeynodeNoRole : public ScKeynode<ScType::NodeConstNoRole>
{
public:
  explicit ScKeynodeNoRole(std::string_view const & sysIdtf)
    : ScKeynode(sysIdtf)
  {
  }
};

/*!
 * @class A base class for const role keynodes. Use it to create keynode in ScKeynodes class to use in agent programs.
 * @example
 * \code
 * static ScKeynodeRole const kRrel1{"rrel_1"};
 * \endcode
 * @warning Use it only for static objects declaration.
 */
class ScKeynodeRole : public ScKeynode<ScType::NodeConstRole>
{
public:
  explicit ScKeynodeRole(std::string_view const & sysIdtf)
    : ScKeynode(sysIdtf)
  {
  }
};

/*!
 * @class A base class for const link keynodes. Use it to create keynode in ScKeynodes class to use in agent programs.
 * @example
 * \code
 * static ScKeynodeRole const kMessageLink{"message_link"};
 * \endcode
 * @warning Use it only for static objects declaration.
 */
class ScKeynodeLink : public ScKeynode<ScType::LinkConst>
{
public:
  explicit ScKeynodeLink(std::string_view const & sysIdtf)
    : ScKeynode(sysIdtf)
  {
  }
};

/*!
 * @class A base class for keynodes declaration. It's like a namespace. Use it as a base class for own keynodes.
 * @example
 * File sc_nlp_keynodes.hpp:
 * \code
 * class ScNLPKeynodes : public ScKeynodes
 * {
 * public:
 *   static inline ScKeynodeClass const kConceptMessage{"concept_message"};
 *   static inline ScKeynodeNoRole const kNrelHistory{"nrel_history"};
 *   static inline ScKeynodeNoRole const kNrelDecomposition{"nrel_decomposition"};
 *   static inline ScKeynodeRole const kRrel1{"rrel_1"};
 * };
 * \endcode
 * @see ScKeynode
 */
class ScKeynodes : public ScObject
{
public:
  _SC_EXTERN sc_result Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) override;
  _SC_EXTERN sc_result Shutdown(ScMemoryContext * ctx) override;

  _SC_EXTERN static inline ScKeynode<ScType::NodeConst> const myself{"myself"};

  _SC_EXTERN static inline ScKeynodeClass const action{"action"};
  _SC_EXTERN static inline ScKeynodeClass const action_state{"action_state"};
  _SC_EXTERN static inline ScKeynodeClass const action_initiated{"action_initiated"};
  _SC_EXTERN static inline ScKeynodeClass const action_finished{"action_finished"};
  _SC_EXTERN static inline ScKeynodeClass const action_finished_successfully{"action_finished_successfully"};
  _SC_EXTERN static inline ScKeynodeClass const action_finished_unsuccessfully{"action_finished_unsuccessfully"};
  _SC_EXTERN static inline ScKeynodeClass const action_finished_with_error{"action_finished_with_error"};
  _SC_EXTERN static inline ScKeynodeNoRole const nrel_answer{"nrel_answer"};

  _SC_EXTERN static inline ScKeynodeClass const sc_result_class{"sc_result"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_ok{"sc_result_ok"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_no{"sc_result_no"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_unknown{"sc_result_unknown"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error{"sc_result_error"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error_invalid_params{"sc_result_error_invalid_params"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error_invalid_type{"sc_result_error_invalid_type"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error_io{"sc_result_error_io"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_invalid_state{"sc_result_error_invalid_state"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error_not_found{"sc_result_error_not_found"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error_full_memory{"sc_result_error_full_memory"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error_addr_is_not_valid{"sc_result_error_addr_is_not_valid"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error_element_is_not_node{
      "sc_result_error_element_is_not_node"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error_element_is_not_link{
      "sc_result_error_element_is_not_link"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error_element_is_not_connector{
      "sc_result_error_element_is_not_corrector"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error_file_memory_io{"sc_result_error_file_memory_io"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error_stream_io{"sc_result_error_stream_io"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error_invalid_system_identifier{
      "sc_result_error_invalid_system_identifier"};
  _SC_EXTERN static inline ScKeynodeClass const sc_result_error_duplicated_system_identifier{
      "sc_result_error_duplicated_system_identifier"};

  _SC_EXTERN static inline ScKeynodeClass const binary_type{"binary_type"};
  _SC_EXTERN static inline ScKeynodeClass const binary_float{"binary_float"};
  _SC_EXTERN static inline ScKeynodeClass const binary_double{"binary_double"};
  _SC_EXTERN static inline ScKeynodeClass const binary_int8{"binary_int8"};
  _SC_EXTERN static inline ScKeynodeClass const binary_int16{"binary_int16"};
  _SC_EXTERN static inline ScKeynodeClass const binary_int32{"binary_int32"};
  _SC_EXTERN static inline ScKeynodeClass const binary_int64{"binary_int64"};
  _SC_EXTERN static inline ScKeynodeClass const binary_uint8{"binary_uint8"};
  _SC_EXTERN static inline ScKeynodeClass const binary_uint16{"binary_uint16"};
  _SC_EXTERN static inline ScKeynodeClass const binary_uint32{"binary_uint32"};
  _SC_EXTERN static inline ScKeynodeClass const binary_uint64{"binary_uint64"};
  _SC_EXTERN static inline ScKeynodeClass const binary_string{"binary_string"};
  _SC_EXTERN static inline ScKeynodeClass const binary_custom{"binary_custom"};

  _SC_EXTERN static inline ScKeynodeClass const empty_class{};

private:
  _SC_EXTERN static ScAddr const & GetResultCodeAddr(sc_result resCode);
  _SC_EXTERN static sc_result GetResultCodeByAddr(ScAddr const & resultClassAddr);
  _SC_EXTERN static ScAddr const & GetRrelIndex(size_t idx);
  _SC_EXTERN static size_t GetRrelIndexNum();
};
