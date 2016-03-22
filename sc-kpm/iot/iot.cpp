/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iot.hpp"
#include "iotKeynodes.hpp"
#include "iotCommands.hpp"

#include "wrap/kpm/sc_agent.hpp"

_SC_EXT_EXTERN sc_result initialize()
{
    if (!ScAgentInit())
        return SC_RESULT_ERROR;

    if (!iot::Keynodes::initGlobal())
		return SC_RESULT_ERROR;

	if (!iot::Commands::initialize())
		return SC_RESULT_ERROR;

	return SC_RESULT_OK;
}

_SC_EXT_EXTERN sc_result shutdown()
{
	sc_result result = SC_RESULT_OK;

	if (!iot::Commands::shutdown())
		result = SC_RESULT_ERROR;	

	/// TODO: shutdown other subsytems

	return result;
}