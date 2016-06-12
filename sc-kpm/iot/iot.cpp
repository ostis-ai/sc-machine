/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotAbout.hpp"
#include "iotActions.hpp"
#include "iotCommands.hpp"
#include "iotContent.hpp"
#include "iotEnergy.hpp"
#include "iotSpeech.hpp"
#include "iotTV.hpp"
#include "iotKeynodes.hpp"

#include "iot.hpp"


using namespace iot;

ActionManager ActionMngr;

SC_IMPLEMENT_MODULE(iotModule)

sc_result iotModule::initializeImpl()
{
	if (!iot::Keynodes::initGlobal())
		return SC_RESULT_ERROR;

	ActionManager::initGlobal();
	ActionMngr.initialize();

	if (!iot::Commands::initialize())
		return SC_RESULT_ERROR;

	return SC_RESULT_OK;
}

sc_result iotModule::shutdownImpl()
{
	sc_result result = SC_RESULT_OK;

	if (!iot::Commands::shutdown())
		result = SC_RESULT_ERROR;

	ActionMngr.shutdown();

	/// TODO: shutdown other subsytems

	return result;
}