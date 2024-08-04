/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <utility>

#include "sc_object.hpp"

#include "sc_addr.hpp"
#include "sc_type.hpp"
#include "sc_template.hpp"

class ScKeynode;

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
  static void Remember(ScKeynode * keynode);

  static void Forget(ScKeynode * keynode);

  /*!
   * @brief Registers all reminded keynodes.
   * @param context Sc-memory context to resolve keynodes.
   */
  static void Register(ScMemoryContext * context, ScAddr initMemoryGeneratedStructure);

  static void Unregister(ScMemoryContext *);

protected:
  static inline std::list<ScKeynode *> m_keynodes;
};
}  // namespace internal

/*!
 * @class A base class for keynodes. Use it to create keynode in ScKeynodes class to use in agent programs.
 * @example
 * \code
 * static inline ScKeynode const keynode{"keynode_const_node", ScType::NodeConst};
 * \endcode
 * @warning Use it only for static objects declaration.
 */
class _SC_EXTERN ScKeynode : public ScAddr
{
public:
  _SC_EXTERN explicit ScKeynode(std::string_view const & sysIdtf = "", ScType const & type = ScType::NodeConst);

  _SC_EXTERN ~ScKeynode();

  _SC_EXTERN ScKeynode(ScKeynode const & other);

  _SC_EXTERN ScKeynode & operator=(ScKeynode const & other);

protected:
  std::string_view m_sysIdtf;
  ScType m_type;

  friend class internal::ScKeynodesRegister;

  virtual void Initialize(ScMemoryContext * context, ScAddr const & initMemoryGeneratedStructureAddr);
};

class _SC_EXTERN ScTemplateKeynode final
  : public ScKeynode
  , protected ScTemplate
{
public:
  _SC_EXTERN explicit ScTemplateKeynode(std::string_view const & sysIdtf = "");

  _SC_EXTERN ~ScTemplateKeynode();

  SC_DISALLOW_COPY(ScTemplateKeynode);

  _SC_EXTERN ScTemplateKeynode(ScTemplateKeynode && other);

  template <typename T1, typename T2, typename T3>
  ScTemplateKeynode Triple(T1 const & param1, T2 const & param2, T3 const & param3) noexcept(false);

  template <typename T1, typename T2, typename T3, typename T4, typename T5>
  ScTemplateKeynode Quintuple(
      T1 const & param1,
      T2 const & param2,
      T3 const & param3,
      T4 const & param4,
      T5 const & param5) noexcept(false);

private:
  std::list<std::function<void(ScTemplate &)>> m_constructionInitializers;

  friend class internal::ScKeynodesRegister;

  template <typename T>
  static auto HandleParam(T const & param);

  void Initialize(ScMemoryContext * context, ScAddr const & initMemoryGeneratedStructureAddr) override;
};

#include "sc_keynodes.tpp"

/*!
 * @class A base class for keynodes declaration. It's like a namespace. Use it as a base class for own keynodes.
 * @example
 * File sc_nlp_keynodes.hpp:
 * \code
 * class ScNLPKeynodes : public ScKeynodes
 * {
 * public:
 *   static inline ScKeynode const kConceptMessage{"concept_message", ScType::NodeConstClass};
 *   static inline ScKeynode const kNrelHistory{"nrel_history", ScType::NodeConstNoRole};
 *   static inline ScKeynode const kNrelDecomposition{"nrel_decomposition", ScType::NodeConstNoRole};
 *   static inline ScKeynode const kRrel1{"rrel_1", ScType::NodeConstRole};
 * };
 * \endcode
 * @see ScKeynode
 */
class _SC_EXTERN ScKeynodes : public ScObject

{
public:
  _SC_EXTERN static inline ScKeynode const myself{"myself"};

  _SC_EXTERN static inline ScKeynode const nrel_inclusion{"nrel_inclusion", ScType::NodeConstNoRole};

  _SC_EXTERN static inline ScKeynode const action{"action", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const action_state{"action_state", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const action_deactivated{"action_deactivated", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const action_initiated{"action_initiated", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const action_finished{"action_finished", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const action_finished_successfully{
      "action_finished_successfully",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const action_finished_unsuccessfully{
      "action_finished_unsuccessfully",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const action_finished_with_error{
      "action_finished_with_error",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const nrel_answer{"nrel_answer", ScType::NodeConstNoRole};

  _SC_EXTERN static inline ScKeynode const abstract_sc_agent{"abstract_sc_agent", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const nrel_primary_initiation_condition{
      "nrel_primary_initiation_condition",
      ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const nrel_sc_agent_action_class{
      "nrel_sc_agent_action_class",
      ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const nrel_initiation_condition_and_result{
      "nrel_initiation_condition_and_result",
      ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const platform_dependent_abstract_sc_agent{
      "platform_dependent_abstract_sc_agent",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const nrel_sc_agent_program{"nrel_sc_agent_program", ScType::NodeConstNoRole};

  _SC_EXTERN static inline ScKeynode const sc_event{"sc_event", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_event_unknown{"sc_event_unknown", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_event_add_input_arc{"sc_event_add_input_arc", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_event_add_output_arc{"sc_event_add_output_arc", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_event_add_edge{"sc_event_add_edge", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_event_remove_input_arc{
      "sc_event_remove_input_arc",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_event_remove_output_arc{
      "sc_event_remove_output_arc",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_event_remove_edge{"sc_event_remove_edge", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_event_remove_element{"sc_event_remove_element", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_event_change_content{"sc_event_change_content", ScType::NodeConstClass};

  _SC_EXTERN static inline ScKeynode const sc_result_class{"sc_result", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_ok{"sc_result_ok", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_no{"sc_result_no", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_unknown{"sc_result_unknown", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error{"sc_result_error", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error_invalid_params{
      "sc_result_error_invalid_params",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error_invalid_type{
      "sc_result_error_invalid_type",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error_io{"sc_result_error_io", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_invalid_state{
      "sc_result_error_invalid_state",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error_not_found{
      "sc_result_error_not_found",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error_full_memory{
      "sc_result_error_full_memory",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error_addr_is_not_valid{
      "sc_result_error_addr_is_not_valid",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error_element_is_not_node{
      "sc_result_error_element_is_not_node",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error_element_is_not_link{
      "sc_result_error_element_is_not_link",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error_element_is_not_connector{
      "sc_result_error_element_is_not_corrector",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error_file_memory_io{
      "sc_result_error_file_memory_io",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error_stream_io{
      "sc_result_error_stream_io",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error_invalid_system_identifier{
      "sc_result_error_invalid_system_identifier",
      ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const sc_result_error_duplicated_system_identifier{
      "sc_result_error_duplicated_system_identifier",
      ScType::NodeConstClass};

  _SC_EXTERN static inline ScKeynode const binary_type{"binary_type", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const binary_float{"binary_float", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const binary_double{"binary_double", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const binary_int8{"binary_int8", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const binary_int16{"binary_int16", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const binary_int32{"binary_int32", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const binary_int64{"binary_int64", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const binary_uint8{"binary_uint8", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const binary_uint16{"binary_uint16", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const binary_uint32{"binary_uint32", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const binary_uint64{"binary_uint64", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const binary_string{"binary_string", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const binary_custom{"binary_custom", ScType::NodeConstClass};

  _SC_EXTERN static ScAddr const & GetRrelIndex(size_t idx);
  _SC_EXTERN static size_t GetRrelIndexNum();

private:
  friend class ScMemory;
  friend class ScModule;
  friend class ScLink;

  static _SC_EXTERN void Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr);
  static _SC_EXTERN void Shutdown(ScMemoryContext * ctx);

  static _SC_EXTERN ScAddr const & GetResultCodeAddr(sc_result resCode);
};
