/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <iomanip>

#include <curl/curl.h>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>


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

bool parseTimeStamp(std::string str, std::tm & outValue)
{
    //2016-02-16T00:30:13.529Z
    tStringVector tokens;
    tokenize(str, tokens, "-T:.");

    check_expr(tokens.size() == 7);
    outValue.tm_year = atoi(tokens[0].c_str());
    outValue.tm_mon = atoi(tokens[1].c_str());
    outValue.tm_mday = atoi(tokens[2].c_str());

    outValue.tm_hour = atoi(tokens[3].c_str());
    outValue.tm_min = atoi(tokens[4].c_str());
    outValue.tm_sec = atoi(tokens[5].c_str());

    return true;
}

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
            tStringMap::const_iterator it = m_parameters.find(name);
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
        tStringMap m_parameters;
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

        parseTimeStamp(jsonValue.GetString(), m_timeStamp);
        
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

    const tStringMap & GetResultParameters() const
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
    tStringMap m_resultParameters;
    apiAi::tContextVector m_contexts;
    apiAi::MetaData m_metaData;
    apiAi::Fulfillment m_fulfillment;
    apiAi::Status m_status;
};

class ApiAiRequest
{
public:
    

    explicit ApiAiRequest()
    {
        m_curl = curl_easy_init();
        curl_easy_setopt(m_curl, CURLOPT_URL, "https://api.api.ai/v1/query?v=20150910");
    }

    ~ApiAiRequest()
    {
        curl_easy_cleanup(m_curl);
    }

    std::string getPostJSON(ApiAiRequestParams const & params)
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

    bool parseResponseJSON(std::string const & responseJSON)
    {
        rapidjson::Document jsonDocument;

        jsonDocument.Parse(responseJSON.c_str());
        if (jsonDocument.HasParseError())
            return false;

        return m_result.parseJSON(jsonDocument);
    }

    bool perform(ApiAiRequestParams const & params)
    {
        bool result = false;
        std::string responseString;

        /// TODO: add support of timezone and location

        std::string const postData = getPostJSON(params);

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
        {
            result = parseResponseJSON(responseString);			
        }

        curl_slist_free_all(headers);

        return result;
    }

    ApiAiRequestResult const & getResult() const
    {
        return m_result;
    }

private:
    CURL * m_curl;

    ApiAiRequestResult m_result;
};
