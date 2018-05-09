/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/cpp/python/sc_python_service.hpp>

namespace http
{

class HttpService : public py::ScPythonService
{
public:
  HttpService();

private:
  virtual void RunImpl() override;
  virtual void StopImpl() override;
};

}