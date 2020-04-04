/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_python_module.hpp"

SC_IMPLEMENT_MODULE(PythonModule)

sc_result PythonModule::InitializeImpl()
{

  m_httpService.reset(new HttpPythonService("http_api/http_api.py"));
  m_httpService->Run();

  return SC_RESULT_OK;
}

sc_result PythonModule::ShutdownImpl()
{
  m_httpService->Stop();
  m_httpService.reset();

  return SC_RESULT_OK;
}
