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
  static void Remember(ScAddr * keynode, std::string_view const & idtf, ScType const & keynodeType);

  static void Forget(ScAddr * keynode);

  /*!
   * @brief Registers all reminded keynodes.
   * @param context Sc-memory context to resolve keynodes.
   */
  static void Register(ScMemoryContext * context, ScAddr initMemoryGeneratedStructure);

  static void Unregister(ScMemoryContext *);

protected:
  static inline std::map<ScAddr *, std::pair<std::string_view, ScType>> m_keynodes;
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
class _SC_EXTERN ScKeynode final : public ScAddr
{
public:
  _SC_EXTERN explicit ScKeynode(std::string_view const & sysIdtf = "", ScType const & type = ScType::NodeConst);

  _SC_EXTERN explicit ScKeynode(sc_addr const & addr);

  _SC_EXTERN ~ScKeynode();

  _SC_EXTERN ScKeynode(ScKeynode const & other);

  _SC_EXTERN ScKeynode & operator=(ScKeynode const & other);
};

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

  _SC_EXTERN static inline ScKeynode const action{"action", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const action_state{"action_state", ScType::NodeConstClass};
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

  _SC_EXTERN static inline ScKeynode const event_add_output_arc{"add_outgoing_edge", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const event_add_input_arc{"add_ingoing_edge", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const event_remove_output_arc{"remove_outgoing_edge", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const event_remove_input_arc{"remove_ingoing_edge", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const event_erase_element{"delete_element", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const event_change_content{"content_change", ScType::NodeConstClass};

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

  _SC_EXTERN static inline ScKeynode const empty_class{};

  _SC_EXTERN static ScAddr const & GetRrelIndex(size_t idx);
  _SC_EXTERN static size_t GetRrelIndexNum();

private:
  friend class ScMemory;
  friend class ScModule;

  _SC_EXTERN sc_result Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) override;
  _SC_EXTERN sc_result Shutdown(ScMemoryContext * ctx) override;

  _SC_EXTERN static ScAddr const & GetResultCodeAddr(sc_result resCode);
  _SC_EXTERN static sc_result GetResultCodeByAddr(ScAddr const & resultClassAddr);
};
