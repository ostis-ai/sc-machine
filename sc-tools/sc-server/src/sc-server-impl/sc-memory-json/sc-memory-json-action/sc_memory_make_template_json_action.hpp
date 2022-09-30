/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

class ScMemoryMakeTemplateJsonAction : public ScMemoryJsonAction
{
protected:
  std::pair<ScTemplate *, ScTemplateParams> GetTemplate(ScMemoryContext * context, ScMemoryJsonPayload payload)
  {
    ScTemplateParams templParams;
    if (payload.is_object())
    {
      auto const rowParams = payload["params"];
      payload = payload["templ"];

      for (auto it = rowParams.cbegin(); it != rowParams.cend(); it++)
      {
        auto const & key = it.key();
        auto const & value = it.value();
        if (value.is_string())
        {
          ScAddr const & addr = context->HelperFindBySystemIdtf(value.get<std::string>());
          templParams.Add(key, addr);
        }
        else
          templParams.Add(key, ScAddr(value.get<size_t>()));
      }
    }

    ScTemplate * scTemplate;
    if (payload.is_string())
    {
      scTemplate = new ScTemplate();
      std::string templateStr = payload.get<std::string>();
      context->HelperBuildTemplate(*scTemplate, templateStr);
    }
    else if (payload.find("type") != payload.end())
    {
      std::string const & type = payload["type"].get<std::string>();
      auto const & value = payload["value"];

      scTemplate = new ScTemplate();
      if (type == "addr")
        context->HelperBuildTemplate(*scTemplate, ScAddr(value.get<size_t>()), templParams);
      else if (type == "idtf")
      {
        ScAddr const & templateStruct = context->HelperFindBySystemIdtf(value.get<std::string>());
        context->HelperBuildTemplate(*scTemplate, templateStruct, templParams);
      }
    }
    else
      scTemplate = MakeTemplate(payload);

    return {scTemplate, templParams};
  }

  ScTemplate * MakeTemplate(ScMemoryJsonPayload const & triples)
  {
    auto const & convertItemToParam = [](ScMemoryJsonPayload paramItem) -> ScTemplateItemValue {
      std::string const & paramType = paramItem["type"].get<std::string>();
      auto const & paramValue = paramItem["value"];

      ScTemplateItemValue param;
      if (paramItem["alias"].is_null())
      {
        if (paramType == "type")
          param = ScType(paramValue.get<size_t>());
        else if (paramType == "addr")
          param = ScAddr(paramValue.get<size_t>());
        else if (paramType == "alias")
          param = paramValue.get<std::string>();
      }
      else
      {
        std::string const alias = paramItem["alias"].get<std::string>();
        if (paramType == "type")
          param = ScType(paramValue.get<size_t>()) >> alias;
        else if (paramType == "addr")
          param = ScAddr(paramValue.get<size_t>()) >> alias;
      }

      return param;
    };

    auto * scTemplate = new ScTemplate();
    for (auto const & triple : triples)
    {
      auto const & srcParam = convertItemToParam(triple[0]);
      auto const & edgeParam = convertItemToParam(triple[1]);
      auto const & trgParam = convertItemToParam(triple[2]);

      scTemplate->Triple(srcParam, edgeParam, trgParam);
    }

    return scTemplate;
  }
};
