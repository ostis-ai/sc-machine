/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_action.hpp"

#include "sc_agent_context.hpp"
#include "sc_result.hpp"
#include "sc_event_wait.hpp"
#include "sc_structure.hpp"

ScAction::ScAction(ScAgentContext * context, ScAddr const & actionAddr) noexcept
  : ScAddr(actionAddr)
  , m_context(context)
  , m_resultAddr(ScAddr::Empty)
  , m_actionClassAddr(ScAddr::Empty)
{
}

ScAddr ScAction::GetClass() const noexcept
{
  if (m_context->IsElement(m_actionClassAddr)
      && m_context->HelperCheckEdge(m_actionClassAddr, *this, ScType::EdgeAccessConstPosPerm))
    return m_actionClassAddr;

  ScIterator3Ptr const it3 = m_context->Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, *this);
  while (it3->Next())
  {
    ScAddr const & actionClassAddr = it3->Get(0);
    if (m_context->HelperCheckEdge(ScKeynodes::action_state, actionClassAddr, ScType::EdgeAccessConstPosPerm))
      continue;

    ScIterator5Ptr const it5 = m_context->Iterator5(
        ScKeynodes::action,
        ScType::EdgeDCommonConst,
        actionClassAddr,
        ScType::EdgeAccessConstPosPerm,
        ScKeynodes::nrel_inclusion);
    if (it5->Next())
    {
      m_actionClassAddr = actionClassAddr;
      break;
    }
  }

  return m_actionClassAddr;
}

ScAddr ScAction::GetArgument(size_t idx, ScAddr const & defaultArgumentAddr) const noexcept
{
  return GetArgument(ScKeynodes::GetRrelIndex(idx), defaultArgumentAddr);
}

ScAddr ScAction::GetArgument(ScAddr const & orderRelationAddr, ScAddr const & defaultArgumentAddr) const noexcept
{
  ScIterator5Ptr const it = m_context->Iterator5(
      *this, ScType::EdgeAccessConstPosPerm, ScType::Unknown, ScType::EdgeAccessConstPosPerm, orderRelationAddr);

  if (it->Next())
    return it->Get(2);

  return defaultArgumentAddr;
}

ScAction & ScAction::SetArgument(size_t idx, ScAddr const & argumentAddr) noexcept(false)
{
  return SetArgument(ScKeynodes::GetRrelIndex(idx), argumentAddr);
}

ScAction & ScAction::SetArgument(ScAddr const & orderRelationAddr, ScAddr const & argumentAddr)
{
  ScIterator5Ptr const it = m_context->Iterator5(
      *this, ScType::EdgeAccessConstPosPerm, ScType::Unknown, ScType::EdgeAccessConstPosPerm, orderRelationAddr);

  while (it->Next())
    m_context->EraseElement(it->Get(1));

  ScAddr const & arcAddr = m_context->CreateEdge(ScType::EdgeAccessConstPosPerm, *this, argumentAddr);
  m_context->CreateEdge(ScType::EdgeAccessConstPosPerm, orderRelationAddr, arcAddr);

  return *this;
}

ScStructure ScAction::GetResult() noexcept(false)
{
  if (!IsInitiated())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get result of action " << GetActionPrettyString() << GetActionClassPrettyString()
                                            << "` because it had not been initiated yet.");

  if (!IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get result of action " << GetActionPrettyString() << GetActionClassPrettyString()
                                            << " because it had not been finished yet.");

  ScIterator5Ptr const & it5 = m_context->Iterator5(
      *this, ScType::EdgeDCommonConst, ScType::Unknown, ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_result);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Action " << GetActionPrettyString() << GetActionClassPrettyString() << " does not have result structure.");

  m_resultAddr = it5->Get(2);
  return m_context->ConvertToStructure(m_resultAddr);
}

ScAction & ScAction::SetResult(ScAddr const & structureAddr) noexcept(false)
{
  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to set result for " << GetActionPrettyString() << GetActionClassPrettyString()
                                      << " because it had already been finished.");

  if (m_resultAddr == structureAddr)
    return *this;

  if (!m_context->IsElement(structureAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to set structure for action " << GetActionPrettyString() << GetActionClassPrettyString()
                                                << " because settable structure is not valid.");

  if (m_context->IsElement(m_resultAddr))
    m_context->EraseElement(m_resultAddr);

  m_resultAddr = structureAddr;
  return *this;
}

bool ScAction::IsInitiated() const noexcept
{
  return m_context->HelperCheckEdge(ScKeynodes::action_initiated, *this, ScType::EdgeAccessConstPosPerm);
}

bool ScAction::InitiateAndWait(sc_uint32 waitTime_ms) noexcept(false)
{
  if (IsInitiated())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to initiate action " << GetActionPrettyString() << GetActionClassPrettyString()
                                       << " because it had already been initiated.");

  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to initiate action " << GetActionPrettyString() << GetActionClassPrettyString()
                                       << " because it had already been finished.");

  auto wait = std::shared_ptr<ScWaiterActionFinished>(new ScWaiterActionFinished(*m_context, *this));
  wait->SetOnWaitStartDelegate(
      [this]()
      {
        m_context->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_initiated, *this);
      });
  return wait->Wait(waitTime_ms);
};

ScAction & ScAction::Initiate() noexcept(false)
{
  if (IsInitiated())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to initiate action " << GetActionPrettyString() << GetActionClassPrettyString()
                                       << " because it had already been initiated.");

  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to initiate action " << GetActionPrettyString() << GetActionClassPrettyString()
                                       << " because it had already been finished.");

  m_context->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_initiated, *this);

  return *this;
};

void ScAction::Finish(ScAddr const & actionStateAddr) noexcept(false)
{
  if (!IsInitiated())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to finish action " << GetActionPrettyString() << GetActionClassPrettyString()
                                     << " because it had not been initiated yet.");

  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to finish action " << GetActionPrettyString() << GetActionClassPrettyString()
                                     << " because it had already been finished.");

  if (!m_context->IsElement(m_resultAddr))
    m_resultAddr = m_context->CreateNode(ScType::NodeConstStruct);

  ScAddr const & arcAddr = m_context->CreateEdge(ScType::EdgeDCommonConst, *this, m_resultAddr);
  m_context->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_result, arcAddr);

  m_context->CreateEdge(ScType::EdgeAccessConstPosPerm, actionStateAddr, *this);
  m_context->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished, *this);
}

bool ScAction::IsFinished() const noexcept
{
  return m_context->HelperCheckEdge(ScKeynodes::action_finished, *this, ScType::EdgeAccessConstPosPerm);
}

bool ScAction::IsFinishedSuccessfully() const noexcept
{
  return m_context->HelperCheckEdge(ScKeynodes::action_finished_successfully, *this, ScType::EdgeAccessConstPosPerm);
}

ScResult ScAction::FinishSuccessfully() noexcept(false)
{
  Finish(ScKeynodes::action_finished_successfully);
  return SC_RESULT_OK;
}

bool ScAction::IsFinishedUnsuccessfully() const noexcept
{
  return m_context->HelperCheckEdge(ScKeynodes::action_finished_unsuccessfully, *this, ScType::EdgeAccessConstPosPerm);
}

ScResult ScAction::FinishUnsuccessfully() noexcept(false)
{
  Finish(ScKeynodes::action_finished_unsuccessfully);
  return SC_RESULT_NO;
}

bool ScAction::IsFinishedWithError() const noexcept
{
  return m_context->HelperCheckEdge(ScKeynodes::action_finished_with_error, *this, ScType::EdgeAccessConstPosPerm);
}

ScResult ScAction::FinishWithError() noexcept(false)
{
  Finish(ScKeynodes::action_finished_with_error);
  return SC_RESULT_ERROR;
}

std::string ScAction::GetActionPrettyString() const
{
  std::string actionName = m_context->HelperGetSystemIdtf(*this);
  if (actionName.empty())
    actionName = std::to_string(this->Hash());

  if (!actionName.empty())
  {
    std::stringstream stream;
    stream << "`" << actionName << "`";
    return stream.str();
  }

  return actionName;
}

std::string ScAction::GetActionClassPrettyString() const
{
  ScAddr const & actionClassAddr = GetClass();
  std::string actionClassName;
  if (actionClassAddr.IsValid())
  {
    actionClassName = m_context->HelperGetSystemIdtf(actionClassAddr);
    if (actionClassName.empty())
      actionClassName = std::to_string(actionClassAddr.Hash());
  }

  if (!actionClassName.empty())
  {
    std::stringstream stream;
    stream << " with class `" << actionClassName << "`";
    return stream.str();
  }

  return actionClassName;
}
