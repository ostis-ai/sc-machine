
#include <iostream>
#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"
#include "scp.hpp"
#include "scpKeynodes.hpp"
#include "scpProcessCreator.hpp"
#include "scpProcessDestroyer.hpp"
#include "scpGenOperatorInterpreter.hpp"

using namespace scp;

SC_IMPLEMENT_MODULE(scpModule)

sc_result scpModule::InitializeImpl()
{
    std::cout << "SCP START" << std::endl;

    if (!scp::Keynodes::InitGlobal())
        return SC_RESULT_ERROR;

    SC_AGENT_REGISTER(ASCPProcessCreator)
    SC_AGENT_REGISTER(ASCPProcessDestroyer)
    SC_AGENT_REGISTER(ASCPGenOperatorInterpreter)

    return SC_RESULT_OK;
}

sc_result scpModule::ShutdownImpl()
{
    std::cout << "SCP END" << std::endl;
    return SC_RESULT_OK;
}
