#include "sc_http_response.hpp"

#include <curl/curl.h>

ScHttpResponse::ScHttpResponse(std::string const & data, uint8_t resultCode)
  : m_data(data)
  , m_resultCode(resultCode)
{
}

std::string const & ScHttpResponse::GetData() const
{
  return m_data;
}

std::string ScHttpResponse::GetResultCodeString() const
{
  return curl_easy_strerror((CURLcode)m_resultCode);
}

bool ScHttpResponse::IsSuccess() const
{
  return ((CURLcode)m_resultCode == CURLE_OK);
}