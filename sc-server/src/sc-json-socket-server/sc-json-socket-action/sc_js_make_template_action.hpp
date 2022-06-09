#pragma once

#include "sc_js_action.hpp"

class ScJSMakeTemplateAction : public ScJSAction
{
protected:
  ScTemplate * getTemplate(ScMemoryContext * context, ScJSPayload payload)
  {
    ScTemplateParams templParams;
    if (payload.find("templ") != payload.end())
    {
      auto const & rowParams = payload["params"];
      payload = payload["templ"];

      for (auto it = rowParams.begin(); it != rowParams.end(); it++)
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

    if (payload.is_string())
    {
      auto * scTemplate = new ScTemplate();
      context->HelperBuildTemplate(*scTemplate, payload.get<std::string>());
      return scTemplate;
    }
    else if (payload.find("type") != payload.end())
    {
      std::string const & type = payload["type"];
      auto const & value = payload["value"];

      auto * scTemplate = new ScTemplate();
      if (type == "addr")
        context->HelperBuildTemplate(*scTemplate, ScAddr(value.get<size_t>()));
      else if (type == "idtf")
      {
        ScAddr const & templateStruct = context->HelperFindBySystemIdtf(value);
        context->HelperBuildTemplate(*scTemplate, templateStruct);
      }

      return scTemplate;
    }
    else
      return makeTemplate(payload);
  }

  ScTemplate * makeTemplate(ScJSPayload const & triples)
  {
    auto const & convertItemToParam = [](ScJSPayload paramItem) -> ScTemplateItemValue {
      std::string const & paramType = paramItem["type"];
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
        if (paramType == "type")
          param = ScType(paramValue.get<size_t>()) >> paramItem["alias"];
        else if (paramType == "addr")
          param = ScAddr(paramValue.get<size_t>()) >> paramItem["alias"];
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
