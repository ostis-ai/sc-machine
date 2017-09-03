/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "bottle.hpp"
#include "bottleKeynodes.hpp"
#include "AOpenValveAgent.hpp"
#include "AFillBottle.hpp"
#include "AUnpressButtonAgent.hpp"

using namespace bottle;

SC_IMPLEMENT_MODULE(bottleModule)

sc_result bottleModule::InitializeImpl()
{
    if (!bottle::Keynodes::InitGlobal())
        return SC_RESULT_ERROR;

    SC_AGENT_REGISTER(AOpenValveAgent)
    SC_AGENT_REGISTER(AFillBottle)
    SC_AGENT_REGISTER(AUnpressButtonAgent)

    return SC_RESULT_OK;
}

sc_result bottleModule::ShutdownImpl()
{
    SC_AGENT_UNREGISTER(AOpenValveAgent)
    SC_AGENT_UNREGISTER(AFillBottle)
    SC_AGENT_UNREGISTER(AUnpressButtonAgent)

    sc_result result = SC_RESULT_OK;
    return result;
}
