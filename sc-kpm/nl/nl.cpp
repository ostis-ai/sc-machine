/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "nl.hpp"
#include "nlApiAi.hpp"
#include "nlIvona.hpp"
#include "nlDialogue.hpp"

#include "nlKeynodes.hpp"

SC_IMPLEMENT_MODULE(nlModule)

sc_result nlModule::initializeImpl()
{
    // keynodes should be initialized first
    nl::Keynodes::initGlobal();

    SC_AGENT_REGISTER(nl::AApiAiParseUserTextAgent);
    SC_AGENT_REGISTER(nl::AIvonaGenerateSpeechAgent);
    SC_AGENT_REGISTER(nl::ADialogueProcessMessageAgent);

    return SC_RESULT_OK;
}

sc_result nlModule::shutdownImpl()
{
    SC_AGENT_UNREGISTER(nl::AApiAiParseUserTextAgent);
    SC_AGENT_UNREGISTER(nl::AIvonaGenerateSpeechAgent);
    SC_AGENT_UNREGISTER(nl::ADialogueProcessMessageAgent);

    return SC_RESULT_OK;
}