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
  public:
    enum class Status : uint8_t
    {
      Error,
      Ok
    };

    explicit Response(Status status, std::string const & data)
      : m_status(status)
      , m_data(data)
    {
    }

    Status GetStatus() const { return m_status; }
    bool IsStatusOk() const { return m_status == Status::Ok; }
    std::string const & GetData() { return m_data; }

  private:
    Status m_status;
    std::string m_data;
  };

  using ResponsePtr = std::shared_ptr<Response>;

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

    ResponsePtr WaitResponse(uint32_t timeOutMS = 10000)
    {
      m_waitResponse.Wait(timeOutMS);
      return m_response;
    }

    /* Just for internal usage */
    void _OnMakeResponse(Response::Status status, std::string const & data)
    {
      m_response.reset(new Response(status, data));
      m_waitResponse.Resolve();
    }
    
  private:
    std::string m_name;
    std::string m_data;
    ResponsePtr m_response;
    ScWait m_waitResponse;
  };

  using RequestPtr = std::shared_ptr<Request>;

public:
  _SC_EXTERN ScPythonBridge();

  _SC_EXTERN ResponsePtr DoRequest(std::string const & eventName, std::string const & data);
  /* Waits until bridge would be initialized
  * Returns true, when bridge initialized correctly; otherwise - false
  */
  _SC_EXTERN bool WaitInitialize(uint32_t timeOutMS = 10000);
  _SC_EXTERN bool IsInitialized() const;
  _SC_EXTERN void Close();

  void Initialize();
  RequestPtr GetNextRequest();

private:
  ScWait m_initWait;
  bool m_isInitialized;

  mutable utils::ScLock m_lock;
  std::list<RequestPtr> m_eventsQueue;
};

using ScPythonBridgePtr = std::shared_ptr<ScPythonBridge>;

} // py