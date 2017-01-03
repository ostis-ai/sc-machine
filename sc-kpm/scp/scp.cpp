
#include "scp.hpp"
#include <iostream>

SC_IMPLEMENT_MODULE(scpModule)

sc_result scpModule::initializeImpl()
{
    std::cout << "SCP START" << std::endl;
    return SC_RESULT_OK;
}

sc_result scpModule::shutdownImpl()
{
    std::cout << "SCP END" << std::endl;
    return SC_RESULT_OK;
}
