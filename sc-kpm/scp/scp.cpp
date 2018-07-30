
#include <iostream>
#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"
#include "scp.hpp"
#include "scpKeynodes.hpp"
#include "scpProcessCreator.hpp"
#include "scpProcessDestroyer.hpp"
#include "scpGenOperatorInterpreter.hpp"
#include "scpEraseOperatorInterpreter.hpp"
#include "scpSearchOperatorInterpreter.hpp"
#include "scpIfOperatorInterpreter.hpp"
#include "scpProgramExecutionSyncronizer.hpp"
#include "scpProcessControlOperatorInterpreter.hpp"

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
    SC_AGENT_REGISTER(ASCPEraseOperatorInterpreter)
    SC_AGENT_REGISTER(ASCPSearchOperatorInterpreter)
    SC_AGENT_REGISTER(ASCPIfOperatorInterpreter)
    SC_AGENT_REGISTER(ASCPProgramExecutionSyncronizer)
    SC_AGENT_REGISTER(ASCPProcessControlOperatorInterpreter)

    return SC_RESULT_OK;
}

sc_result scpModule::ShutdownImpl()
{
    std::cout << "SCP END" << std::endl;
    SC_AGENT_UNREGISTER(ASCPProcessCreator)
    SC_AGENT_UNREGISTER(ASCPProcessDestroyer)
    SC_AGENT_UNREGISTER(ASCPGenOperatorInterpreter)
    SC_AGENT_UNREGISTER(ASCPEraseOperatorInterpreter)
    SC_AGENT_UNREGISTER(ASCPSearchOperatorInterpreter)
    SC_AGENT_UNREGISTER(ASCPIfOperatorInterpreter)
    SC_AGENT_UNREGISTER(ASCPProgramExecutionSyncronizer)
    SC_AGENT_UNREGISTER(ASCPProcessControlOperatorInterpreter)

    return SC_RESULT_OK;
}
