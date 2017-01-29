/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "nlApiAiUtils.hpp"

extern "C"
{
#include "sc-memory/sc-store/sc_config.h"
}

namespace
{

size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, std::string * s)
{
  size_t newLength = size * nmemb;
  size_t oldLength = s->size();
  try
  {
    s->resize(oldLength + newLength);
  }
  catch (std::bad_alloc & e)
  {
    //handle memory problem
    return 0;
  }

  std::copy((char*)contents, (char*)contents + newLength, s->begin() + oldLength);
  return size * nmemb;
}

} // namespace


ApiAiRequest::ApiAiRequest()
{
  m_curl = curl_easy_init();
  curl_easy_setopt(m_curl, CURLOPT_URL, "https://api.api.ai/v1/query?v=20150910");
}

ApiAiRequest::~ApiAiRequest()
{
  curl_easy_cleanup(m_curl);
}

std::string ApiAiRequest::GetPostJSON(ApiAiRequestParams const & params)
{
  rapidjson::Document jsonDocument;
  rapidjson::Document::AllocatorType & jsonAllocator = jsonDocument.GetAllocator();
  jsonDocument.SetObject();

  rapidjson::Value jsonQuery;
  jsonQuery.SetArray();

  rapidjson::Value jsonQueryValue;
  jsonQueryValue.SetString(params.m_queryString.c_str(), (rapidjson::SizeType)params.m_queryString.size(), jsonAllocator);
  jsonQuery.PushBack(jsonQueryValue, jsonAllocator);

  rapidjson::Value jsonSessionId;
  jsonSessionId.SetString(params.m_sessionId.c_str(), (rapidjson::SizeType)params.m_sessionId.size(), jsonAllocator);

  rapidjson::Value jsonLang;
  jsonLang.SetString(params.m_lang.c_str(), (rapidjson::SizeType)params.m_lang.size(), jsonAllocator);

  jsonDocument.AddMember("query", jsonQuery, jsonAllocator);
  jsonDocument.AddMember("sessionId", jsonSessionId, jsonAllocator);
  jsonDocument.AddMember("lang", jsonLang, jsonAllocator);

  rapidjson::StringBuffer strBuffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);
  jsonDocument.Accept(writer);

  return std::string(strBuffer.GetString());
}

bool ApiAiRequest::ParseResponseJSON(std::string const & responseJSON)
{
  rapidjson::Document jsonDocument;

  jsonDocument.Parse(responseJSON.c_str());
  if (jsonDocument.HasParseError())
    return false;

  return m_result.parseJSON(jsonDocument);
}

bool ApiAiRequest::Perform(ApiAiRequestParams const & params)
{
  bool result = false;
  std::string responseString;

  /// TODO: add support of timezone and location

  std::string const postData = GetPostJSON(params);

  curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
  curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, postData.c_str());

  curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, 0L);

  std::string const accessToken = sc_config_get_value_string("apiai", "access_token");
  std::stringstream headersString;
  headersString << "Authorization: Bearer " << accessToken;

  curl_slist * headers = nullptr;
  headers = curl_slist_append(headers, headersString.str().c_str());
  headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");
  curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
  curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &responseString);

  // perform request
  CURLcode resultCode = curl_easy_perform(m_curl);
  std::string const error = curl_easy_strerror(resultCode);
  if (resultCode == CURLE_OK)
    result = ParseResponseJSON(responseString);

  curl_slist_free_all(headers);

  return result;
}

