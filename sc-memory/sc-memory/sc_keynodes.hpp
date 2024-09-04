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
 * @class ScKeynodesRegister
 * @brief A base class to initialize/destroy keynodes. It stores keynodes and resolves them after memory initialization.
 * @warning This class is for internal usage only.
 */
class ScKeynodesRegister
{
public:
  /*!
   * @brief Stores keynode data to initialize it later.
   * @param keynode A pointer to the keynode data to be stored.
   */
  static void Remember(ScKeynode * keynode);

  /*!
   * @brief Removes keynode data from the stored list.
   * @param keynode A pointer to the keynode data to be removed.
   */
  static void Forget(ScKeynode * keynode);

  /*!
   * @brief Initializes all stored keynodes.
   * @param context A sc-memory context to resolve keynodes.
   */
  static void Register(ScMemoryContext * context);

  /*!
   * @brief Destroys all keynodes.
   * @param context A sc-memory context used for destroying keynodes.
   */
  static void Unregister(ScMemoryContext * context);

protected:
  static inline std::list<ScKeynode *> m_notInitializedKeynodes;
  static inline std::list<ScKeynode *> m_initializedKeynodes;
};

}  // namespace internal

/*!
 * @class ScKeynode
 * @brief A base class for keynodes. Use it to generate keynodes in the ScKeynodes class for use in agent programs.
 *
 * A keynode is a key element used by some sc-agent during its operation. Typically, keynodes can be classes and
 * relations, which are sc-elements that agents use to find constructions in sc-memory or generate new ones. However,
 * keynodes can be any sc-elements, even connectors between sc-elements.
 *
 * @note Objects of this class can be used as ScAddr.
 * @warning Use this class only for static object declarations.
 * @warning Keynodes can be defined not only in classes derived from the `ScKeynodes` class. You can define them as
 * static objects anywhere.
 *
 * @code
 * static inline ScKeynode const my_keynode_class{"my_keynode_class", ScType::NodeConstClass};
 * @endcode
 */
class _SC_EXTERN ScKeynode : public ScAddr
{
  friend class internal::ScKeynodesRegister;

public:
  /*!
   * @brief Defines a keynode with the provided system identifier `sysIdtf` and sc-type `type`.
   * @param sysIdtf A system identifier of the sc-keynode to be resolved.
   * @param type A sc-type of the sc-keynode to be resolved.
   */
  _SC_EXTERN explicit ScKeynode(
      std::string_view const & sysIdtf = "",
      ScType const & type = ScType::NodeConst) noexcept;

  _SC_EXTERN ~ScKeynode() noexcept;

  _SC_EXTERN operator std::string() const noexcept;

protected:
  std::string_view m_sysIdtf;
  ScType m_type;

  /*!
   * @brief Initializes the keynode with the given context and memory structure address.
   * @param context A sc-memory context used for initialization.
   */
  virtual void Initialize(ScMemoryContext * context);
};

/*!
 * @class ScTemplateKeynode
 * @brief A base class to generate sc-templates as keynodes. Use it to generate keynodes of sc-templates in the
 * ScKeynodes class for use in agent programs.
 *
 * This class is useful when you want use programably represented sc-template in different places in the code or when
 * you don't want specify sc-template in knowledge base and use it in `ScAgentBuilder`.
 *
 * @note Objects of this class can be used as ScTemplate and ScAddr.
 * @warning Use this class only for static object declarations.
 *
 * @code
 * static inline ScTemplateKeynode const & my_agent_initiation_condition =
 *   ScTemplateKeynode("my_agent_initiation_condition")
 *     .Triple(
 *        ScKeynodes::action_initiated,
 *        ScType::EdgeAccessVarPosPerm,
 *        ScType::NodeVar >> "_action")
 *     .Triple(
 *        MyKeynodes::my_action,
 *        ScType::EdgeAccessVarPosPerm,
 *        "_action");
 * @endcode
 */
class _SC_EXTERN ScTemplateKeynode final
  : public ScKeynode
  , protected ScTemplate
{
  friend class internal::ScKeynodesRegister;

  SC_DISALLOW_COPY(ScTemplateKeynode);

public:
  /*!
   * @brief Defines a keynode for sc-template with the provided system identifier `sysIdtf`.
   * @param sysIdtf A system identifier of the sc-template keynode to be resolved.
   */
  _SC_EXTERN explicit ScTemplateKeynode(std::string_view const & sysIdtf = "") noexcept;

  _SC_EXTERN ~ScTemplateKeynode() noexcept;

  _SC_EXTERN ScTemplateKeynode(ScTemplateKeynode && other) noexcept;

  /*!
   * @brief Appends provided triple to sc-template keynode.
   * @note This method can be used in the same way as a method `Triple` of the class `ScTemplate`.
   */
  template <typename T1, typename T2, typename T3>
  _SC_EXTERN ScTemplateKeynode Triple(T1 const & param1, T2 const & param2, T3 const & param3) noexcept;

  /*!
   * @brief Appends provided quintuple to sc-template keynode.
   * @note This method can be used in the same way as a method `Quintuple` of the class `ScTemplate`.
   */
  template <typename T1, typename T2, typename T3, typename T4, typename T5>
  _SC_EXTERN ScTemplateKeynode
  Quintuple(T1 const & param1, T2 const & param2, T3 const & param3, T4 const & param4, T5 const & param5) noexcept;

private:
  std::list<std::function<void(ScTemplate &)>> m_constructionInitializers;

  template <typename T>
  static auto HandleParam(T const & param) noexcept;

  /*!
   * @brief Initializes the keynode with the given context and memory structure address.
   * @param context A sc-memory context used for initialization.
   */
  void Initialize(ScMemoryContext * context) override;
};

#include "sc_keynodes.tpp"

/*!
 * @class ScKeynodes
 * @brief A base class for keynodes declaration. It's like a namespace. Use it as a base class for own keynodes.
 * @note This class already contains frequently used keynodes. Use them in your code.
 *
 * @code
 * // File my_keynodes.hpp:
 * #pragma once
 *
 * #include <sc-memory/sc_keynodes.hpp>
 *
 * class MyKeynodes : public ScKeynodes
 * {
 * public:
 *   static inline ScKeynode const my_keynode_class_a{"my_keynode_class_a", ScType::NodeConstClass};
 *   static inline ScKeynode const my_keynode_relation_b{"my_keynode_relation_b", ScType::NodeConstNoRole};
 * };
 * @endcode
 */
class _SC_EXTERN ScKeynodes : public ScObject
{
  friend class ScMemory;
  friend class ScModule;
  friend class ScLink;

public:
  _SC_EXTERN static inline ScKeynode const myself{"myself"};

  _SC_EXTERN static inline ScKeynode const nrel_inclusion{"nrel_inclusion", ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const rrel_1{"rrel_1", ScType::NodeConstRole};
  _SC_EXTERN static inline ScKeynode const rrel_2{"rrel_2", ScType::NodeConstRole};
  _SC_EXTERN static inline ScKeynode const rrel_3{"rrel_3", ScType::NodeConstRole};
  _SC_EXTERN static inline ScKeynode const nrel_basic_sequence{"nrel_basic_sequence", ScType::NodeConstNoRole};

  _SC_EXTERN static inline ScKeynode const rrel_key_sc_element{"rrel_key_sc_element", ScType::NodeConstRole};
  _SC_EXTERN static inline ScKeynode const rrel_main_key_sc_element{"rrel_main_key_sc_element", ScType::NodeConstRole};

  _SC_EXTERN static inline ScKeynode const nrel_idtf{"nrel_idtf", ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const nrel_main_idtf{"nrel_main_idtf", ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const lang_ru{"lang_ru", ScType::NodeConstClass};

  _SC_EXTERN static inline ScKeynode const action{"action", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const receptor_action{"receptor_action", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const effector_action{"effector_action", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const behavioral_action{"behavioral_action", ScType::NodeConstClass};
  _SC_EXTERN static inline ScKeynode const information_action{"information_action", ScType::NodeConstClass};
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
  _SC_EXTERN static inline ScKeynode const nrel_result{"nrel_result", ScType::NodeConstNoRole};

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
  _SC_EXTERN static inline ScKeynode const sc_event_unknown{"sc_event_unknown", ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const sc_event_after_generate_connector{
      "sc_event_after_generate_connector",
      ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const sc_event_after_generate_incoming_arc{
      "sc_event_after_generate_incoming_arc",
      ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const sc_event_after_generate_outgoing_arc{
      "sc_event_after_generate_outgoing_arc",
      ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const sc_event_after_generate_edge{
      "sc_event_after_generate_edge",
      ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const sc_event_before_erase_connector{
      "sc_event_before_erase_connector",
      ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const sc_event_before_erase_incoming_arc{
      "sc_event_before_erase_incoming_arc",
      ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const sc_event_before_erase_outgoing_arc{
      "sc_event_before_erase_outgoing_arc",
      ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const sc_event_before_erase_edge{
      "sc_event_before_erase_edge",
      ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const sc_event_before_erase_element{
      "sc_event_before_erase_element",
      ScType::NodeConstNoRole};
  _SC_EXTERN static inline ScKeynode const sc_event_before_change_link_content{
      "sc_event_before_change_link_content",
      ScType::NodeConstNoRole};

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
  static _SC_EXTERN void Initialize(ScMemoryContext * context);
  static _SC_EXTERN void Shutdown(ScMemoryContext * context);

  static _SC_EXTERN ScAddr const & GetResultCodeAddr(sc_result resCode);
};
