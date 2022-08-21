#pragma once

#include <string>
#include <vector>

#include "sc_http_response.hpp"

#include "../sc_defines.hpp"

class ScHttpRequest
{
public:
  enum class Type : uint8_t
  {
    GET,
    POST,
    PUT
  };

  explicit _SC_EXTERN ScHttpRequest(std::string const & url = "");
  _SC_EXTERN ~ScHttpRequest();

  _SC_EXTERN void Perform();

  _SC_EXTERN void SetURL(std::string const & url);
  _SC_EXTERN void SetType(Type type);
  // Set data for a POST and PUT requests
  _SC_EXTERN void SetData(std::string const & data);

  _SC_EXTERN void SetHeaders(std::vector<std::string> && headers);
  _SC_EXTERN void AddHeader(std::string const & header);

  _SC_EXTERN ScHttpResponsePtr GetResponse() const;

private:
  void * m_handle;
  Type m_type;
  std::string m_data;
  std::vector<std::string> m_headers;
  ScHttpResponsePtr m_response;
};
