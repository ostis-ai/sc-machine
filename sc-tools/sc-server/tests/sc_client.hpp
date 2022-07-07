#pragma once

#include <nlohmann/json.hpp>

#include "sc_client_defines.hpp"

using ScMemoryJsonPayload = nlohmann::json;

#define WAIT_SERVER std::this_thread::sleep_for(std::chrono::milliseconds(100))

class ScClient
{
public:
  ScClient()
  {
    m_instance = new ScClientCore();

    Initialize();
  }

  sc_bool Connect(std::string const & uri)
  {
    ScClientErrorCode code;
    m_connection = m_instance->get_connection(uri, code);

    if (code.value())
      return SC_FALSE;

    m_instance->connect(m_connection);

    return SC_TRUE;
  }

  void Run()
  {
    m_thread = std::thread(&ScClientCore::run, &*m_instance);
  }

  void Stop()
  {
    m_instance->stop();
    m_thread.join();
  }

  sc_bool Send(std::string const & msg)
  {
    ScClientErrorCode code;
    m_instance->send(m_connection, msg, ScServerMessageType::text, code);

    return !code;
  }

  void OnMessage(ScServerConnectionHandle const & hdl, ScServerMessage const & msg)
  {
    m_currentPayload = ScMemoryJsonPayload::parse(msg->get_payload())["payload"];
  }

  ScMemoryJsonPayload GetResponsePayload()
  {
    return m_currentPayload;
  }

  ~ScClient()
  {
    delete m_instance;
  }

private:
  ScClientCore * m_instance;
  ScClientConnection m_connection;
  std::thread m_thread;
  ScMemoryJsonPayload m_currentPayload;

  void Initialize()
  {
    m_instance->clear_access_channels(ScServerLogMessages::all);
    m_instance->set_error_channels(ScServerLogErrors::all);

    m_instance->init_asio();

    m_instance->set_message_handler(bind(&ScClient::OnMessage, this, ::_1, ::_2));
  }
};