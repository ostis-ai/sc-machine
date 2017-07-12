#include "sc_python_bridge.hpp"

namespace py
{

ScPythonBridge::ScPythonBridge()
  : m_isInitialized(false)
{
}


ScPythonBridge::ResponsePtr ScPythonBridge::DoRequest(std::string const & eventName, std::string const & data)
{
  RequestPtr req(new Request(eventName, data));
  {
    utils::ScLockScope lock(m_lock);
    m_eventsQueue.push_back(req);
  }
  return req->WaitResponse();
}

bool ScPythonBridge::WaitInitialize(uint32_t timeOutMS/* = 10000*/)
{
  return m_initWait.Wait(timeOutMS);
}

bool ScPythonBridge::IsInitialized() const
{
  utils::ScLockScope lock(m_lock);

  return m_isInitialized;
}

void ScPythonBridge::Close()
{
  utils::ScLockScope lock(m_lock);

  m_isInitialized = false;
}

void ScPythonBridge::Initialize()
{
  utils::ScLockScope lock(m_lock);

  m_initWait.Resolve();
  m_isInitialized = true;
}

ScPythonBridge::RequestPtr ScPythonBridge::GetNextRequest()
{
  RequestPtr request;
  {
    utils::ScLockScope lock(m_lock);

    if (m_eventsQueue.empty())
      return RequestPtr();

    request = m_eventsQueue.front();
    m_eventsQueue.pop_front();
  }

  return request;
}

}