/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "http_api_service.hpp"

namespace http
{

HttpService::HttpService()
  : ScPythonService("http_api/http_api.py")
{
}

void HttpService::RunImpl()
{

}

void HttpService::StopImpl()
{

}

}