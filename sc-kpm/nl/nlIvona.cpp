/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "nlIvona.hpp"

#include "sc-memory/cpp/sc_stream.hpp"
#include "sc-memory/cpp/sc_struct.hpp"

#include "hash/hmac.h"
#include "hash/sha256.h"

#include <curl/curl.h>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <iomanip>

namespace
{
    size_t CurlWrite_CallbackFunc_StdVector(void *contents, size_t size, size_t nmemb, std::vector<uint8_t> * data)
    {
        size_t newLength = size * nmemb;

        uint8_t * data_ptr = (uint8_t*)contents;

        data->insert(data->end(), data_ptr, data_ptr + newLength);

        return size * nmemb;
    }

    std::string _sha256(std::string const & data)
    {
        SHA256 sha256;
        sha256.add(data.data(), data.size());
        return sha256.getHash();
    }

    std::string _convertToHex(unsigned char * buffer, size_t bufferSize)
    {
        // convert to hex string
        std::string result;
        result.reserve(2 * bufferSize);
        for (size_t i = 0; i < bufferSize; i++)
        {
            static const char dec2hex[16 + 1] = "0123456789abcdef";
            result += dec2hex[(buffer[i] >> 4) & 15];
            result += dec2hex[buffer[i] & 15];
        }

        return result;
    }

    std::string _signKey(std::string const & key, std::string const & signString, std::string const & dateStamp, std::string const & regionName, std::string const & serviceName)
    {
        std::string const value = std::string("AWS4") + key;
        uint8_t keyDate[SHA256::HashBytes];
        hmac<SHA256>(value, dateStamp, keyDate);

        uint8_t keyRegion[SHA256::HashBytes];
        hmac<SHA256>(keyDate, SHA256::HashBytes, regionName.c_str(), regionName.size(), keyRegion);

        uint8_t keyService[SHA256::HashBytes];
        hmac<SHA256>(keyRegion, SHA256::HashBytes, serviceName.c_str(), serviceName.size(), keyService);

        uint8_t keySigning[SHA256::HashBytes];
        std::string const requestStr = "aws4_request";
        hmac<SHA256>(keyService, SHA256::HashBytes, requestStr.c_str(), requestStr.size(), keySigning);

        uint8_t signature[SHA256::HashBytes];
        hmac<SHA256>(keySigning, SHA256::HashBytes, signString.c_str(), signString.size(), signature);

        std::string const result = _convertToHex(signature, SHA256::HashBytes);

        return result.c_str();
    }

    class IvonaApi final
    {
    public:
        explicit IvonaApi()
        {
        }

        ~IvonaApi()
        {
        }

        bool generateSpeech(std::string const & text, std::string const & language, std::string const & voiceName, std::vector<uint8_t> & buffer)
        {
            std::string const regionName = "global";
            std::string const serviceName = "tts";
            std::string const accessKey = sc_config_get_value_string("ivona", "access_key");
            std::string const secretKey = sc_config_get_value_string("ivona", "secret_key");

            rapidjson::Document jsonDocument;
            rapidjson::Document::AllocatorType & jsonAllocator = jsonDocument.GetAllocator();
            jsonDocument.SetObject();

            // Create input object
            rapidjson::Value jsonInput;
            {
                jsonInput.SetObject();

                rapidjson::Value jsonInputData;
                jsonInputData.SetString(text.c_str(), jsonAllocator);

                jsonInput.AddMember("Data", jsonInputData, jsonAllocator);
            }

            // Create output format object
            rapidjson::Value jsonOutputFormat;
            {
                jsonOutputFormat.SetObject();

                rapidjson::Value jsonOutputFormatValue;
                jsonOutputFormatValue.SetString("OGG", jsonAllocator);

                jsonOutputFormat.AddMember("Codec", jsonOutputFormatValue, jsonAllocator);
            }

            // Create voice object
            rapidjson::Value jsonVoice;
            {
                jsonVoice.SetObject();

                rapidjson::Value jsonVoiceName;
                jsonVoiceName.SetString(voiceName.c_str(), jsonAllocator);

                rapidjson::Value jsonVoiceLang;
                jsonVoiceLang.SetString(language.c_str(), jsonAllocator);

                jsonVoice.AddMember("Name", jsonVoiceName, jsonAllocator);
                jsonVoice.AddMember("Language", jsonVoiceLang, jsonAllocator);
            }
            
            // Create rate object
            rapidjson::Value jsonParams;
            {
                jsonParams.SetObject();

                rapidjson::Value jsonRateValue;
                jsonRateValue.SetString("medium", jsonAllocator);

                jsonParams.AddMember("Rate", jsonRateValue, jsonAllocator);
            }

            jsonDocument.AddMember("Input", jsonInput, jsonAllocator);
            jsonDocument.AddMember("OutputFormat", jsonOutputFormat, jsonAllocator);
            jsonDocument.AddMember("Parameters", jsonParams, jsonAllocator);
            jsonDocument.AddMember("Voice", jsonVoice, jsonAllocator);
            
            rapidjson::StringBuffer strJsonData;
            rapidjson::Writer<rapidjson::StringBuffer> writer(strJsonData);
            jsonDocument.Accept(writer);

            
            std::string const dataSha256 = _sha256(strJsonData.GetString());
            
            std::time_t t = std::time(nullptr);
            std::tm tm = *std::localtime(&t);

            std::stringstream dateStampStream;
            dateStampStream << tm.tm_year
                            << std::setw(2) << std::setfill('0') << tm.tm_mon
                            << std::setw(2) << std::setfill('0') << tm.tm_mday;
                
            std::string const dateStamp = dateStampStream.str();

            std::stringstream dateTimeStream;
            dateTimeStream
                << dateStamp << "T"
                << std::setw(2) << std::setfill('0') << tm.tm_hour
                << std::setw(2) << std::setfill('0') << tm.tm_min
                << std::setw(2) << std::setfill('0') << tm.tm_sec
                << "Z";
            std::string const dateTime = dateTimeStream.str();

            std::string url = serviceName + "." + regionName + ".ivonacloud.com";
            
            // hash
            std::string canonicalRequest;
            canonicalRequest += "POST\n";
            canonicalRequest += "/CreateSpeech\n\n";
            canonicalRequest += "content-type:application/json\n";
            canonicalRequest += "host:" + url + "\n";
            canonicalRequest += "x-amz-content-sha256:" + dataSha256 + "\n";
            canonicalRequest += "x-amz-date:" + dateTime + "\n\n";
            canonicalRequest += "content-type;host;x-amz-content-sha256;x-amz-date\n";
            canonicalRequest += dataSha256;

            std::string const requestHash = _sha256(canonicalRequest);

            // sign 
            std::string signData = "AWS4-HMAC-SHA256\n";
            std::string const signData2 = dateTime + "\n";
            std::string const signData3 = dateStamp + "/" + regionName + "/" + serviceName + "/aws4_request\n";
            signData = signData + signData2 + signData3 + requestHash;

            std::string const signature = _signKey(secretKey, signData, dateStamp, regionName, serviceName);

            std::string authorithationHeader = "AWS4-HMAC-SHA256 Credential=";
            authorithationHeader += accessKey + "/";
            authorithationHeader += dateStamp + "/";
            authorithationHeader += regionName + "/tts/aws4_request";
            authorithationHeader += ", SignedHeaders=content-type;host;x-amz-content-sha256;x-amz-date";
            authorithationHeader += ", Signature=" + signature;

            // do request
            CURL * curl = curl_easy_init();
            curl_easy_setopt(curl, CURLOPT_URL, "https://tts.global.ivonacloud.com/CreateSpeech");
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strJsonData.GetString());

            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            curl_slist * headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            std::string hValue = "X-Amz-Date: " + dateTime;
            headers = curl_slist_append(headers, hValue.c_str());
            hValue = "Authorization: " + authorithationHeader;
            headers = curl_slist_append(headers, hValue.c_str());
            hValue = "x-amz-content-sha256: " + dataSha256;
            headers = curl_slist_append(headers, hValue.c_str());

            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdVector);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

            // perform request
            CURLcode resultCode = curl_easy_perform(curl);
            std::string const error = curl_easy_strerror(resultCode);
            if (resultCode == CURLE_OK)
            {
                /// TODO: check we got audio file
                return true;
            }

            curl_slist_free_all(headers);

            return false;
        }

    private:
        

    };
}


namespace nl
{
    ScAddr AIvonaGenerateSpeechAgent::msFormatOgg;
    ScAddr AIvonaGenerateSpeechAgent::msNrelFormat;

    SC_AGENT_ACTION_IMPLEMENTATION(AIvonaGenerateSpeechAgent)
    {
        curl_global_init(CURL_GLOBAL_ALL);

        // get text to process
        ScIterator3Ptr iter = mMemoryCtx.iterator3(requestAddr, *ScType::EDGE_ACCESS_CONST_POS_PERM, *ScType::LINK);
        if (iter->next())
        {
            ScStream stream;
            if (mMemoryCtx.getLinkContent(iter->value(2), stream))
            {
                std::string text;
                if (StreamConverter::streamToString(stream, text))
                {
                    IvonaApi api;
                    /// TODO: determine language and voice
                    std::vector<uint8_t> soundData;
                    if (api.generateSpeech(text, "en-GB", "Brian", soundData))
                    {
                        ScAddr linkAddr = mMemoryCtx.createLink();
                        check_expr(linkAddr.isValid());

                        // append format
                        {
                            ScAddr edge = mMemoryCtx.createEdge(*ScType::EDGE_DCOMMON_CONST, linkAddr, msFormatOgg);
                            check_expr(edge.isValid());
                            mMemoryCtx.createEdge(*ScType::EDGE_ACCESS_CONST_POS_PERM, msNrelFormat, edge);
                        }

                        mMemoryCtx.createEdge(*ScType::EDGE_ACCESS_CONST_POS_PERM, resultAddr, linkAddr);

                        ScStream stream((char*)soundData.data(), (sc_uint32)soundData.size(), SC_STREAM_FLAG_READ);
                        if (mMemoryCtx.setLinkContent(linkAddr, stream))
                            return SC_RESULT_OK;
                    }
                }
                
            }
            else
            {
                return SC_RESULT_ERROR;
            }
            
        }

        

        return SC_RESULT_ERROR;
    }
    

    void runTest()
    {
        IvonaApi api;
        /// TODO: determine language and voice
        std::vector<uint8_t> soundData;
        api.generateSpeech("Hello world", "en-GB", "Brian", soundData);
    }

} // namespace nl
