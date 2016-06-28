/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "nlApiAi.hpp"

#include "wrap/sc_stream.hpp"

#include <curl/curl.h>

struct sPostData
{
	std::string const * data;
	size_t body_size;
	size_t body_pos;

	sPostData(std::string const * inData)
		: data(inData)
		, body_pos(0)
	{
		body_size = data->size();
	}
};

size_t CurlRead_CallbackFunc_StdString(void *ptr, size_t size, size_t nmemb, void *stream)
{
	if (stream)
	{
		sPostData * ud = (sPostData*)stream;

		size_t available = (ud->body_size - ud->body_pos);

		if (available > 0)
		{
			size_t written = min(size * nmemb, available);
			memcpy(ptr, ((char*)(ud->data->data())) + ud->body_pos, written);
			ud->body_pos += written;
			return written;
		}
	}

	return 0;
}

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

	bool perform(std::string & requestString)
	{
		bool result = false;
		std::string responseString;

		/// TODO: add support of timezone and location
		
		std::string postData;

		/// test
		postData = "{ \"query\": [ \"weather in Minsk\"	] }";

		
		curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
		curl_easy_setopt(m_curl, CURLOPT_READFUNCTION, CurlRead_CallbackFunc_StdString);
		curl_easy_setopt(m_curl, CURLOPT_READDATA, &sPostData(&postData));

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
			result = true;
		}

		curl_slist_free_all(headers);

		return result;
	}

private:
	CURL * m_curl;
};

namespace nl
{

	/** 
	  * Command template: requestAddr _-> _text;;
	  */
	SC_AGENT_ACTION_IMPLEMENTATION(AApiAiParseUserTextAgent)
	{
		curl_global_init(CURL_GLOBAL_ALL);

		// get text content of argument
		ScIterator3Ptr iter = mMemoryCtx.iterator3(requestAddr, SC_TYPE(sc_type_arc_pos_const_perm), SC_TYPE(sc_type_link));
		if (iter->next())
		{
			ScStream stream;
			if (mMemoryCtx.getLinkContent(iter->value(2), stream))
			{
				std::string text;
				if (StreamConverter::streamToString(stream, text))
				{
					ApiAiRequest request;

					if (request.perform(text))
					{
						printf("test");
					}
				}
				else
				{
					return SC_RESULT_ERROR_IO;
				}
			}
			else
			{
				return SC_RESULT_ERROR_INVALID_STATE;
			}
		}

		return SC_RESULT_ERROR_INVALID_PARAMS;
	}
}