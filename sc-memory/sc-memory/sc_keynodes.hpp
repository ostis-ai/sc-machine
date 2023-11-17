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
 * @class A base class to register keynodes
 * @brief It reminds keynodes data and resolve them after memory initialization.
 * @note This class is just for internal usage.
 */
class ScKeynodesRegister
{
public:
  static internal::ScKeynodesRegister m_instance;

  /*!
   * @brief Reminds keynode data to register it after.
   * @param keynode Pointer to reminding keynode data
   * @param idtf Keynode system identifier
   * @param keynodeType Keynode syntactic type
   */
  void Remind(ScAddr * keynode, std::string const & idtf, ScType const & keynodeType)
  {
    m_keynodes.insert({keynode, {idtf, keynodeType}});
  }

  /*!
   * @brief Registers all reminded keynodes.
   * @param context Sc-memory context to resolve keynodes
   */
  void Register(ScMemoryContext * context, ScAddr initMemoryGeneratedStructure)
  {
    for (auto const & item : m_keynodes)
    {
      ScAddr * keynode = item.first;
      auto const & keynodeInfo = item.second;
      ScSystemIdentifierFiver fiver;
      context->HelperResolveSystemIdtf(keynodeInfo.first, keynodeInfo.second, fiver);
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

protected:
  std::map<ScAddr *, std::pair<std::string, ScType>> m_keynodes;
};
}  // namespace internal

/*!
 * @class A base class for keynodes. Use it to create keynode in ScKeynodes class to use in agent programs.
 * @example
 * \code
 * static ScKeynode<sc_type_node | sc_type_const> const keynode{"keynode_const_node"};
 * \endcode
 * @warning Use it only for static objects declaration.
 */
template <sc_type type>
class ScKeynode : public ScAddr
{
public:
  explicit ScKeynode<type>(std::string const & sysIdtf)
    : ScAddr(ScAddr::Empty)
  {
    internal::ScKeynodesRegister::m_instance.Remind(this, sysIdtf, ScType(type));
  }

  explicit ScKeynode(sc_addr const & addr)
    : ScAddr(addr)
  {
  }

  ScKeynode(ScKeynode const & other)
    : ScAddr(other)
  {
    m_realAddr = other.m_realAddr;
  }

  ScKeynode & operator=(ScKeynode const & other)
  {
    return *this;
  }

protected:
};

/*!
 * @class A base class for const class keynodes. Use it to create keynode in ScKeynodes class to use in agent programs.
 * @example
 * \code
 * static ScKeynodeClass const kQuestionClass{"question_class"};
 * \endcode
 * @warning Use it only for static objects declaration.
 */
class ScKeynodeClass : public ScKeynode<sc_type_node | sc_type_const | sc_type_node_class>
{
public:
  explicit ScKeynodeClass()
    : ScKeynode(SC_ADDR_EMPTY)
  {
  }

  explicit ScKeynodeClass(std::string const & sysIdtf)
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
class ScKeynodeNoRole : public ScKeynode<sc_type_node | sc_type_const | sc_type_node_norole>
{
public:
  explicit ScKeynodeNoRole(std::string const & sysIdtf)
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
class ScKeynodeRole : public ScKeynode<sc_type_node | sc_type_const | sc_type_node_role>
{
public:
  explicit ScKeynodeRole(std::string const & sysIdtf)
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
class ScKeynodeLink : public ScKeynode<sc_type_link | sc_type_const>
{
public:
  explicit ScKeynodeLink(std::string const & sysIdtf)
    : ScKeynode(sysIdtf)
  {
  }
};

/// Defines `std::string GetName()` method for own keynodes class. It is used in sc-memory logging.
#define SC_KEYNODES_BODY(__KeynodesName) \
  std::string GetName() override \
  { \
    return #__KeynodesName; \
  }

/*!
 * @class A base class for keynodes declaration. It's like a namespace. Use it as a base class for own keynodes.
 * @example
 * File sc_nlp_keynodes.hpp:
 * \code
 * class ScNLPKeynodes : public ScKeynodes
 * {
 * public:
 *    SC_KEYNODES_BODY(ScNLPKeynodes) // define `std::string GetName()` method for class ScNLPKeynodes
 *
 *    static ScKeynodeClass const kConceptMessage;
 *    static ScKeynodeNoRole const kNrelHistory;
 *    static ScKeynodeNoRole const kNrelDecomposition;
 *    static ScKeynodeRole const kRrel1;
 * };
 * \endcode
 * File *.cpp:
 * \code
 * ScKeynodeClass const ScNLPKeynodes::MyModuleKeynodes::kConceptMessage("concept_message");
 * ScKeynodeNoRole const ScNLPKeynodes::kNrelHistory("nrel_history");
 * ScKeynodeNoRole const ScNLPKeynodes::kNrelDecomposition("nrel_decomposition");
 * ScKeynodeRole const ScNLPKeynodes::kRrel1("rrel_1");
 * \endcode
 * @see ScKeynode
 */
class ScKeynodes : public ScObject
{
public:
  SC_KEYNODES_BODY(ScKeynodes)

  _SC_EXTERN sc_result Initialize() override;
  _SC_EXTERN sc_result Initialize(std::string const & initMemoryGeneratedStructure) override;
  _SC_EXTERN sc_result Shutdown() override;

  _SC_EXTERN static ScAddr const & GetResultCodeAddr(sc_result resCode);
  _SC_EXTERN static sc_result GetResultCodeByAddr(ScAddr const & resultClassAddr);
  _SC_EXTERN static ScAddr const & GetRrelIndex(size_t idx);
  _SC_EXTERN static size_t GetRrelIndexNum();

  /// command keynodes
  _SC_EXTERN static ScKeynodeClass const kQuestionState;
  _SC_EXTERN static ScKeynodeClass const kQuestionInitiated;
  _SC_EXTERN static ScKeynodeClass const kQuestionProgressed;
  _SC_EXTERN static ScKeynodeClass const kQuestionStopped;
  _SC_EXTERN static ScKeynodeClass const kQuestionFinished;
  _SC_EXTERN static ScKeynodeClass const kQuestionFinishedSuccessfully;
  _SC_EXTERN static ScKeynodeClass const kQuestionFinishedUnsuccessfully;
  _SC_EXTERN static ScKeynodeClass const kQuestionFinishedWithError;
  _SC_EXTERN static ScKeynodeNoRole const kNrelAnswer;

  /// result codes
  _SC_EXTERN static ScKeynodeClass const kScResult;
  _SC_EXTERN static ScKeynodeClass const kScResultOk;
  _SC_EXTERN static ScKeynodeClass const kScResultNo;
  _SC_EXTERN static ScKeynodeClass const kScResultUnknown;
  _SC_EXTERN static ScKeynodeClass const kScResultError;
  _SC_EXTERN static ScKeynodeClass const kScResultErrorInvalidParams;
  _SC_EXTERN static ScKeynodeClass const kScResultErrorInvalidType;
  _SC_EXTERN static ScKeynodeClass const kScResultErrorIO;
  _SC_EXTERN static ScKeynodeClass const kScResultInvalidState;
  _SC_EXTERN static ScKeynodeClass const kScResultErrorNotFound;

  /// link binary types
  _SC_EXTERN static ScKeynodeClass const kBinaryType;
  _SC_EXTERN static ScKeynodeClass const kBinaryFloat;
  _SC_EXTERN static ScKeynodeClass const kBinaryDouble;
  _SC_EXTERN static ScKeynodeClass const kBinaryInt8;
  _SC_EXTERN static ScKeynodeClass const kBinaryInt16;
  _SC_EXTERN static ScKeynodeClass const kBinaryInt32;
  _SC_EXTERN static ScKeynodeClass const kBinaryInt64;
  _SC_EXTERN static ScKeynodeClass const kBinaryUInt8;
  _SC_EXTERN static ScKeynodeClass const kBinaryUInt16;
  _SC_EXTERN static ScKeynodeClass const kBinaryUInt32;
  _SC_EXTERN static ScKeynodeClass const kBinaryUInt64;
  _SC_EXTERN static ScKeynodeClass const kBinaryString;
  _SC_EXTERN static ScKeynodeClass const kBinaryCustom;

  _SC_EXTERN static ScKeynodeClass const kEmptyClass;
};
