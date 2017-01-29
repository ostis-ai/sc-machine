/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_debug.hpp"

#include <ctime>

#include <curl/curl.h>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

template <typename ParentType>
bool getJSONMemberSafe(ParentType & parent, const char * name, rapidjson::Value & outValue)
{
  if (parent.HasMember(name))
  {
    outValue = parent[name];
    return true;
  }
  return false;
}

template < class ContainerT >
void tokenize(const std::string& str, ContainerT& tokens,
              const std::string& delimiters = " ", bool trimEmpty = false)
{
  std::string::size_type pos, lastPos = 0;

  using value_type = typename ContainerT::value_type;
  using size_type = typename ContainerT::size_type;

  while (true)
  {
    pos = str.find_first_of(delimiters, lastPos);
    if (pos == std::string::npos)
    {
      pos = str.length();

      if (pos != lastPos || !trimEmpty)
        tokens.push_back(value_type(str.data() + lastPos,
                                    (size_type)pos - lastPos));

      break;
    }
    else
    {
      if (pos != lastPos || !trimEmpty)
        tokens.push_back(value_type(str.data() + lastPos,
                                    (size_type)pos - lastPos));
    }

    lastPos = pos + 1;
  }
}

struct TimeStampParser
{
  bool operator () (std::string str, std::tm & outValue) const
  {
    //2016-02-16T00:30:13.529Z
    StringVector tokens;
    tokenize(str, tokens, "-T:.");

    SC_ASSERT(tokens.size() == 7, ());
    outValue.tm_year = atoi(tokens[0].c_str());
    outValue.tm_mon = atoi(tokens[1].c_str());
    outValue.tm_mday = atoi(tokens[2].c_str());

    outValue.tm_hour = atoi(tokens[3].c_str());
    outValue.tm_min = atoi(tokens[4].c_str());
    outValue.tm_sec = atoi(tokens[5].c_str());

    return true;
  }
};

namespace apiAi
{

class Context
{
public:
  explicit Context()
    : m_lifespan(0)
  {
  }

  void addParam(std::string const & name, std::string const & value)
  {
    m_parameters[name] = value;
  }

  std::string const & getParam(std::string const & name) const
  {
    StringMap::const_iterator it = m_parameters.find(name);
    if (it != m_parameters.end())
      return it->second;

    static std::string empty;
    return empty;
  }

  std::string const & getName() const
  {
    return m_name;
  }

  uint32_t getLifespan() const
  {
    return m_lifespan;
  }

  void parseJSON(rapidjson::Value & jsonElement)
  {
    rapidjson::Value jsonValue = rapidjson::Value();
    if (getJSONMemberSafe(jsonElement, "name", jsonValue) && jsonValue.IsString())
      m_name = jsonValue.GetString();

    if (getJSONMemberSafe(jsonElement, "lifespan", jsonValue) && jsonValue.IsInt())
      m_lifespan = jsonValue.GetInt();

    if (getJSONMemberSafe(jsonElement, "parameters", jsonValue) && jsonValue.IsObject())
    {
      rapidjson::Value::MemberIterator iter;
      for (iter = jsonValue.MemberBegin(); iter != jsonValue.MemberEnd(); ++iter)
      {
        if (iter->name.IsString() && iter->value.IsString())
          m_parameters[iter->name.GetString()] = iter->value.GetString();
      }
    }
  }

private:
  std::string m_name;
  StringMap m_parameters;
  uint32_t m_lifespan;
};
typedef std::vector<Context> tContextVector;

/// ---------------
class MetaData
{
public:
  explicit MetaData()
  {
  }

  std::string const & getIntentId() const
  {
    return m_intentId;
  }

  std::string const & getIntentName() const
  {
    return m_intentName;
  }

  void parseJSON(rapidjson::Value & jsonElement)
  {
    rapidjson::Value jsonValue = rapidjson::Value();
    if (getJSONMemberSafe(jsonElement, "intentId", jsonValue) && jsonValue.IsString())
      m_intentId = jsonValue.GetString();

    if (getJSONMemberSafe(jsonElement, "intentName", jsonValue) && jsonValue.IsString())
      m_intentName = jsonValue.GetString();
  }

private:
  std::string m_intentId;
  std::string m_intentName;
};

/// ----------------
class Fulfillment
{
public:
  explicit Fulfillment()
  {
  }

  std::string const & getSpeech() const
  {
    return m_speech;
  }

  std::string const & getSource() const
  {
    return m_source;
  }

  void parseJSON(rapidjson::Value & jsonElement)
  {
    rapidjson::Value jsonValue = rapidjson::Value();
    if (getJSONMemberSafe(jsonElement, "speech", jsonValue) && jsonValue.IsString())
      m_speech = jsonValue.GetString();

    if (getJSONMemberSafe(jsonElement, "source", jsonValue) && jsonValue.IsString())
      m_source = jsonValue.GetString();
  }

private:
  std::string m_speech;
  std::string m_source;
};

class Status
{
public:
  explicit Status()
    : m_code(0)
  {
  }

  uint32_t getCode() const
  {
    return m_code;
  }

  std::string const & getErrorType() const
  {
    return m_errorType;
  }

  std::string const & getErrorId() const
  {
    return m_errorId;
  }

  std::string const & getErrorDetails() const
  {
    return m_errorDetails;
  }

  void parseJSON(rapidjson::Value & jsonElement)
  {
    rapidjson::Value jsonValue = rapidjson::Value();
    if (getJSONMemberSafe(jsonElement, "code", jsonValue) && jsonValue.IsInt())
      m_code = jsonValue.GetInt();

    if (getJSONMemberSafe(jsonElement, "errorType", jsonValue) && jsonValue.IsString())
      m_errorType = jsonValue.GetString();

    if (getJSONMemberSafe(jsonElement, "errorId", jsonValue) && jsonValue.IsString())
      m_errorId = jsonValue.GetString();

    if (getJSONMemberSafe(jsonElement, "errorDetails", jsonValue) && jsonValue.IsString())
      m_errorDetails = jsonValue.GetString();
  }

private:
  uint32_t m_code;
  std::string m_errorType;
  std::string m_errorId;
  std::string m_errorDetails;
};

} // namespace apiAi


struct ApiAiRequestParams
{
  std::string m_queryString;
  std::string m_sessionId;
  std::string m_lang;
};

class ApiAiRequestResult
{
public:
  explicit ApiAiRequestResult()
  {
  }

  bool parseJSON(rapidjson::Document & jsonDocument)
  {
    /// TODO: parse error

    // request id
    rapidjson::Value jsonValue = rapidjson::Value();

    if (!getJSONMemberSafe(jsonDocument, "id", jsonValue) || !jsonValue.IsString())
      return false;

    m_id = jsonValue.GetString();

    // timestamp
    if (!getJSONMemberSafe(jsonDocument, "timestamp", jsonValue) || !jsonValue.IsString())
      return false;

    TimeStampParser()(jsonValue.GetString(), m_timeStamp);

    // result
    rapidjson::Value jsonResult = rapidjson::Value();
    if (!getJSONMemberSafe(jsonDocument, "result", jsonResult) || !jsonResult.IsObject())
      return false;

    // result source
    if (getJSONMemberSafe(jsonResult, "source", jsonValue) && jsonValue.IsString())
      m_resultSource = jsonValue.GetString();

    // result resolved query
    if (getJSONMemberSafe(jsonResult, "resolvedQuery", jsonValue) && jsonValue.IsString())
      m_resolvedQuery = jsonValue.GetString();

    // result action
    if (getJSONMemberSafe(jsonResult, "action", jsonValue) && jsonValue.IsString())
      m_action = jsonValue.GetString();

    // result actionIncomplete
    if (getJSONMemberSafe(jsonResult, "actionIncomplete", jsonValue) && jsonValue.IsBool())
      m_isActionIncomplete = jsonValue.GetBool();

    // result parameters
    if (getJSONMemberSafe(jsonResult, "parameters", jsonValue) && jsonValue.IsObject())
    {
      rapidjson::Value::ConstMemberIterator iter;
      for (iter = jsonValue.MemberBegin(); iter != jsonValue.MemberEnd(); ++iter)
      {
        if (iter->name.IsString() && iter->value.IsString())
          m_resultParameters[iter->name.GetString()] = iter->value.GetString();
      }
    }

    // contexts
    if (getJSONMemberSafe(jsonResult, "contexts", jsonValue) && jsonValue.IsArray())
    {
      rapidjson::Value::Array const arr = jsonValue.GetArray();
      m_contexts.resize(arr.Size());
      for (size_t i = 0; i < m_contexts.size(); ++i)
      {
        m_contexts[i].parseJSON(arr[(rapidjson::SizeType)i]);
      }
    }

    // metadata
    if (getJSONMemberSafe(jsonResult, "metadata", jsonValue) && jsonValue.IsObject())
      m_metaData.parseJSON(jsonValue);

    // fulfillment
    if (getJSONMemberSafe(jsonResult, "fulfillment", jsonValue) && jsonValue.IsObject())
      m_fulfillment.parseJSON(jsonValue);

    // status
    if (getJSONMemberSafe(jsonDocument, "status", jsonValue) && jsonValue.IsObject())
      m_status.parseJSON(jsonValue);

    return true;
  }

  const std::string & GetId() const
  {
    return m_id;
  }

  const std::tm & GetTimeStamp() const
  {
    return m_timeStamp;
  }

  const std::string & GetResultSource() const
  {
    return m_resultSource;
  }

  const std::string & GetResolverQuery() const
  {
    return m_resolvedQuery;
  }

  const std::string & GetAction() const
  {
    return m_action;
  }

  bool IsActionComplete() const
  {
    return m_isActionIncomplete;
  }

  const StringMap & GetResultParameters() const
  {
    return m_resultParameters;
  }

  const apiAi::tContextVector & GetContexts() const
  {
    return m_contexts;
  }

  const apiAi::MetaData & GetMetaData() const
  {
    return m_metaData;
  }

  const apiAi::Fulfillment & GetFullfillment() const
  {
    return m_fulfillment;
  }

  const apiAi::Status & GetStatus() const
  {
    return m_status;
  }

private:
  std::string m_id;
  std::tm m_timeStamp;
  std::string m_resultSource;
  std::string m_resolvedQuery;
  std::string m_action;
  bool m_isActionIncomplete;
  StringMap m_resultParameters;
  apiAi::tContextVector m_contexts;
  apiAi::MetaData m_metaData;
  apiAi::Fulfillment m_fulfillment;
  apiAi::Status m_status;
};

class ApiAiRequest final
{
public:
  explicit ApiAiRequest();
  ~ApiAiRequest();

  std::string GetPostJSON(ApiAiRequestParams const & params);

  bool ParseResponseJSON(std::string const & responseJSON);
  bool Perform(ApiAiRequestParams const & params);

  ApiAiRequestResult const & GetResult() const { return m_result; }

private:
  CURL * m_curl;

  ApiAiRequestResult m_result;
};
