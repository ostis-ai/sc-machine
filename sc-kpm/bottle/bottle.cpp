/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "bottle.hpp"
#include "bottleKeynodes.hpp"
#include "AOpenValveAgent.hpp"

using namespace bottle;

SC_IMPLEMENT_MODULE(bottleModule)

sc_result bottleModule::InitializeImpl()
{
    if (!bottle::Keynodes::InitGlobal())
        return SC_RESULT_ERROR;

    SC_AGENT_REGISTER(AOpenValveAgent)

    return SC_RESULT_OK;
}

sc_result bottleModule::ShutdownImpl()
{
    sc_result result = SC_RESULT_OK;
    return result;
}
