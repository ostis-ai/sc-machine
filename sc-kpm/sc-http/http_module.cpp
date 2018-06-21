/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "http_module.hpp"

SC_IMPLEMENT_MODULE(httpModule)

sc_result httpModule::InitializeImpl()
{
  SC_ASSERT(m_service.get() == nullptr, ());
  m_service.reset(new http::HttpService());
  m_service->Run();

  return SC_RESULT_OK;
}

sc_result httpModule::ShutdownImpl()
{
  m_service->Stop();
  m_service.release();

  return SC_RESULT_OK;
}