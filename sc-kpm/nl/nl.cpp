/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "nl.hpp"
#include "nlApiAi.hpp"
#include "nlIvona.hpp"

SC_IMPLEMENT_MODULE(nlModule)

sc_result nlModule::initializeImpl()
{
	SC_AGENT_REGISTER(nl::AApiAiParseUserTextAgent);
	SC_AGENT_REGISTER(nl::AIvonaGenerateSpeechAgent);

	//nl::runTest();

	return SC_RESULT_OK;
}

sc_result nlModule::shutdownImpl()
{
	SC_AGENT_UNREGISTER(nl::AApiAiParseUserTextAgent);
	SC_AGENT_UNREGISTER(nl::AIvonaGenerateSpeechAgent);

	return SC_RESULT_OK;
}