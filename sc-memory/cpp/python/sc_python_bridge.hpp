#pragma once

#include "../sc_types.hpp"
#include "../sc_utils.hpp"
#include "../sc_wait.hpp"

#include "../utils/sc_lock.hpp"

namespace py
{

class ScPythonBridge
{
public:
  class Response
  {
  protected:
    explicit Response(std::string const & data)
      : m_data(data)
    {
    }

    std::string GetData() { return m_data; }

  private:
    std::string m_data;
  };

  SHARED_PTR_TYPE(Response)

  class Request
  {
  public:
    Request(std::string const & name, std::string const & data)
      : m_name(name)
      , m_data(data)
    {
    }

    ResponsePtr GetResponse() { return m_response; }

    std::string const & GetName() const { return m_name; }
    std::string const & GetData() const { return m_data; }
    ResponsePtr GetResponse() const { return m_response; }

  private:
    std::string m_name;
    std::string m_data;
    ResponsePtr m_response;
  };

  SHARED_PTR_TYPE(Request)

public:
  _SC_EXTERN ScPythonBridge();

  _SC_EXTERN void SendEvent(std::string const & eventName, std::string const & data);
  /* Waits until bridge would be initialized
  * Returns true, when bridge initialized correctly; otherwise - false
  */
  _SC_EXTERN bool WaitInitialize(uint32_t timeOutMS = 10000);
  _SC_EXTERN bool IsInitialized() const;
  _SC_EXTERN void Close();

  void Initialize();
  RequestPtr GetNextEvent();

private:
  ScWait m_initWait;
  bool m_isInitialized;

  mutable utils::ScLock m_lock;
  std::list<RequestPtr> m_eventsQueue;
};

SHARED_PTR_TYPE(ScPythonBridge)

} // py