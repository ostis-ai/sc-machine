/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <utility>

#include "sc_server_action.hpp"
#include "sc_server.hpp"

class ScServerConnectAction : public ScServerAction
{
public:
  using ScClientURIResourceParams = std::unordered_map<std::string, std::string>;

  ScServerConnectAction(ScServer * server, ScServerSessionId sessionId, std::string resourceData)
    : ScServerAction(std::move(sessionId))
    , m_server(server)
    , m_resourceData(std::move(resourceData))
  {
  }

  ScClientURIResourceParams ParseQueryData(std::string const & queryString)
  {
    ScClientURIResourceParams paramMap;

    size_t pos = 0;
    while (pos < queryString.length())
    {
      size_t endPos = queryString.find('&', pos);

      std::string param = queryString.substr(pos, endPos - pos);
      size_t equalsPos = param.find('=');

      std::string key = param.substr(0, equalsPos);
      std::string value = param.substr(equalsPos + 1);

      paramMap[key] = value;

      if (endPos == std::string::npos)
        break;
      pos = endPos + 1;
    }

    return paramMap;
  }

  ScClientURIResourceParams ParseResourceData()
  {
    ScClientURIResourceParams paramMap;

    size_t queryPos = m_resourceData.find('?');
    if (queryPos != std::string::npos)
    {
      std::string const queryString = m_resourceData.substr(queryPos + 1);
      paramMap = ParseQueryData(queryString);
    }

    return paramMap;
  }

  ScAddr ParseUserAddr(ScClientURIResourceParams const & params)
  {
    ScAddr userAddr;
    if (params.empty())
      return userAddr;

    std::string const & userAddrKey = "user_addr";
    if (params.find(userAddrKey) != params.cend())
    {
      std::string const & userAddrHashString = params.at(userAddrKey);

      ScAddr::HashType userAddrHash;
      std::stringstream stream;
      stream << userAddrHashString;
      stream >> userAddrHash;
      userAddr = ScAddr{userAddrHash};
    }

    return userAddr;
  }

  void Emit() override
  {
    ScClientURIResourceParams params = ParseResourceData();
    ScAddr userAddr = ParseUserAddr(params);
    if (!userAddr.IsValid())
      userAddr = ScKeynodes::kMySelf;

    auto * sessionCtx = new ScMemoryContext(userAddr);
    m_server->AddSessionContext(m_sessionId, sessionCtx);
  }

  ~ScServerConnectAction() override = default;

protected:
  ScServer * m_server;
  std::string m_resourceData;
};
