
#include <iostream>
#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include "mineco.hpp"
#include "minecoPlanCreator.hpp"
#include "minecoPatternVerifier.hpp"

using namespace mineco;

SC_IMPLEMENT_MODULE(minecoModule)

sc_result minecoModule::InitializeImpl()
{

    SC_AGENT_REGISTER(AMinEcoPlanCreator)
    SC_AGENT_REGISTER(AMinEcoPatternVerifier)

    return SC_RESULT_OK;
}

sc_result minecoModule::ShutdownImpl()
{
    SC_AGENT_UNREGISTER(AMinEcoPlanCreator)
    SC_AGENT_UNREGISTER(AMinEcoPatternVerifier)

    return SC_RESULT_OK;
}
