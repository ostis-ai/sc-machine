/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "nlApiAi.hpp"

#include "wrap/sc_stream.hpp"

#include <curl/curl.h>

size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, std::string * s)
{
	size_t newLength = size * nmemb;
	size_t oldLength = s->size();
	try
	{
		s->resize(oldLength + newLength);
	}
	catch (std::bad_alloc &e)
	{
		//handle memory problem
		return 0;
	}

	std::copy((char*)contents, (char*)contents + newLength, s->begin() + oldLength);
	return size * nmemb;
}

namespace nl
{

	/** 
	  * Command templaterequestAddr _-> _text;;
	  */
	SC_AGENT_ACTION_IMPLEMENTATION(AApiAiParseUserTextAgent)
	{
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
					CURL * curl = curl_easy_init();
					if (curl)
					{
						std::stringstream requestString;
						std::string responseString;

						/// TODO: add support of timezone and location
						/// TODO: do sessionID build based on user info
						requestString << "https://api.api.ai/v1/query?query=" << text
							<< "&v=20150910" << "&sessionId=" << SC_ADDR_LOCAL_TO_INT(requestAddr.getRealAddr());

						curl_easy_setopt(curl, CURLOPT_URL, requestString.str().c_str());
						
						std::stringstream headersString;
						headersString << "Authorization: Bearer " << sc_config_get_value_string("apiai", "access_token");

						curl_slist * headers = nullptr;
						headers = curl_slist_append(headers, headersString.str().c_str());
						
						curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
						curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
						curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

						// perform request
						if (curl_easy_perform(curl) == CURLE_OK)
						{
							
						}

						curl_slist_free_all(headers);
						headers = nullptr;

						return SC_RESULT_ERROR_IO;
					}
					else
					{
						return SC_RESULT_ERROR;
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