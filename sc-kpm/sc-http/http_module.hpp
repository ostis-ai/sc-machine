/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "http_api_service.hpp"

#include <sc-memory/cpp/sc_module.hpp>

#include "http_module.generated.hpp"

class httpModule : public ScModule
{
  SC_CLASS(LoadOrder(10))
  SC_GENERATED_BODY()

  virtual sc_result InitializeImpl() override;
  virtual sc_result ShutdownImpl() override;

private:
  std::unique_ptr<http::HttpService> m_service;
};