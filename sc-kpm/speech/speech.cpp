/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "speech.hpp"
#include "ASpeechTranslate.hpp"
#include "AVerificationAction.hpp"
#include "AVerificationRelation.hpp"

using namespace speech;

SC_IMPLEMENT_MODULE(speechModule)

sc_result speechModule::InitializeImpl()
{

    SC_AGENT_REGISTER(ASpeechTranslate)
    SC_AGENT_REGISTER(AVerificationAction)
    SC_AGENT_REGISTER(AVerificationRelation)

    return SC_RESULT_OK;
}

sc_result speechModule::ShutdownImpl()
{
    SC_AGENT_UNREGISTER(ASpeechTranslate)
    SC_AGENT_UNREGISTER(AVerificationAction)
    SC_AGENT_UNREGISTER(AVerificationRelation)

    sc_result result = SC_RESULT_OK;
    return result;
}
