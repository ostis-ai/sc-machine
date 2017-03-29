/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "nl.hpp"
#include "nlApiAi.hpp"
#include "nlIvona.hpp"
#include "nlDialogue.hpp"
#include "nlGenText.hpp"
#include "nlGenCmdTextResult.hpp"

#include "nlTextTemplProcessor.hpp"

#include "nlKeynodes.hpp"

SC_IMPLEMENT_MODULE(nlModule)

sc_result nlModule::InitializeImpl()
{
  // keynodes should be initialized first
  nl::Keynodes::InitGlobal();

  SC_AGENT_REGISTER(nl::AApiAiParseUserTextAgent);
  SC_AGENT_REGISTER(nl::AIvonaGenerateSpeechAgent);
  SC_AGENT_REGISTER(nl::ADialogueProcessMessageAgent);
  
  nl::util::TextTemplProcessor::InitGlobal();
  SC_AGENT_REGISTER(nl::AGenText);
  SC_AGENT_REGISTER(nl::AGenCmdTextResult);

  return SC_RESULT_OK;
}

sc_result nlModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(nl::AApiAiParseUserTextAgent);
  SC_AGENT_UNREGISTER(nl::AIvonaGenerateSpeechAgent);
  SC_AGENT_UNREGISTER(nl::ADialogueProcessMessageAgent);
  SC_AGENT_UNREGISTER(nl::AGenText);
  SC_AGENT_UNREGISTER(nl::AGenCmdTextResult);

  return SC_RESULT_OK;
}
