/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_action.hpp"

#include "sc_agent_context.hpp"

ScAction::ScAction(ScAddr const & userAddr, ScAddr const & actionAddr, ScAddr const & actionClassAddr)
  : ScAddr(actionAddr)
  , m_ctx(new ScAgentContext(userAddr))
  , m_userAddr(userAddr)
  , m_actionClassAddr(actionClassAddr)
  , m_answer(*m_ctx) {};

ScAction::ScAction(ScAgentContext * ctx, ScAddr const & actionClassAddr)
  : ScAddr(ctx->CreateNode(ScType::NodeConst))
  , m_ctx(ctx)
  , m_userAddr(ctx->GetUserAddr())
  , m_actionClassAddr(actionClassAddr)
  , m_answer(*m_ctx)
{
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, m_actionClassAddr, *this);
};

ScAddr ScAction::GetClass() const
{
  return m_actionClassAddr;
}

ScAddr ScAction::GetArgument(size_t idx, ScAddr const & defaultArgumentAddr) const
{
  return GetArgument(ScKeynodes::GetRrelIndex(idx), defaultArgumentAddr);
}

ScAddr ScAction::GetArgument(ScAddr const & orderRelationAddr, ScAddr const & defaultArgumentAddr) const
{
  ScIterator5Ptr const it = m_ctx->Iterator5(
      *this, ScType::EdgeAccessConstPosPerm, ScType::Unknown, ScType::EdgeAccessConstPosPerm, orderRelationAddr);

  if (it->Next())
    return it->Get(2);

  return defaultArgumentAddr;
}

ScAction & ScAction::SetArgument(size_t idx, ScAddr const & argumentAddr)
{
  return SetArgument(ScKeynodes::GetRrelIndex(idx), argumentAddr);
}

ScAction & ScAction::SetArgument(ScAddr const & orderRelationAddr, ScAddr const & argumentAddr)
{
  ScIterator5Ptr const it = m_ctx->Iterator5(
      *this, ScType::EdgeAccessConstPosPerm, ScType::Unknown, ScType::EdgeAccessConstPosPerm, orderRelationAddr);

  if (it->Next())
    m_ctx->EraseElement(it->Get(2));

  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, *this, argumentAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, orderRelationAddr, edgeAddr);

  return *this;
}

ScStruct ScAction::GetAnswer() const
{
  return m_answer;
}

sc_bool ScAction::IsInitiated()
{
  return m_ctx->HelperCheckEdge(ScKeynodes::action_initiated, *this, ScType::EdgeAccessConstPosPerm);
}

std::shared_ptr<ScWaitActionFinished> ScAction::Initiate() noexcept(false)
{
  if (IsInitiated())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash() << "` had already been initiated.");

  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash() << "` had already been finished.");

  auto wait = std::make_shared<ScWaitActionFinished>(*m_ctx, *this);
  wait->SetOnWaitStartDelegate(
      [this]()
      {
        m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_initiated, *this);
      });
  return wait;
};

sc_bool ScAction::IsFinished() const
{
  return m_ctx->HelperCheckEdge(ScKeynodes::action_finished, *this, ScType::EdgeAccessConstPosPerm);
}

sc_bool ScAction::IsFinishedSuccessfully() const
{
  return m_ctx->HelperCheckEdge(ScKeynodes::action_finished_successfully, *this, ScType::EdgeAccessConstPosPerm);
}

sc_result ScAction::FinishSuccessfully() noexcept(false)
{
  if (!IsInitiated())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash() << "` wasn't initiated yet.");

  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash() << "` had already been initiated.");

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished_successfully, *this);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished, *this);
  return SC_RESULT_OK;
}

sc_bool ScAction::IsFinishedUnsuccessfully() const
{
  return m_ctx->HelperCheckEdge(ScKeynodes::action_finished_unsuccessfully, *this, ScType::EdgeAccessConstPosPerm);
}

sc_result ScAction::FinishUnsuccessfully() noexcept(false)
{
  if (!IsInitiated())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash() << "` wasn't initiated yet.");

  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash() << "` had already been initiated.");

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished_unsuccessfully, *this);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished, *this);
  return SC_RESULT_NO;
}

sc_bool ScAction::IsFinishedWithError() const
{
  return m_ctx->HelperCheckEdge(ScKeynodes::action_finished_with_error, *this, ScType::EdgeAccessConstPosPerm);
}

sc_result ScAction::FinishWithError() noexcept(false)
{
  if (!IsInitiated())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash() << "` wasn't initiated yet.");

  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash() << "` had already been initiated.");

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished_with_error, *this);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished, *this);
  return SC_RESULT_ERROR;
}

ScActionEvent::ScActionEvent(
    ScAddr const & userAddr,
    ScAddr const & subscriptionAddr,
    ScAddr const & connectorAddr,
    ScType const & connectorType,
    ScAddr const & otherAddr)
  : ScEventAddOutputEdge(userAddr, subscriptionAddr, connectorAddr, connectorType, otherAddr)
{
}

ScAction ScActionEvent::GetAction() const
{
  return ScAction{m_userAddr, m_otherAddr, m_subscriptionAddr};
}

ScActionEventSubscription::ScActionEventSubscription(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<sc_result(ScActionEvent const &)> const & func)
  : ScElementaryEventSubscription<ScActionEvent>(ctx, addr, func) {};
