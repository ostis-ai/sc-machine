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
      payload = payload["templ"];
      auto const & rowParams = payload["params"];

      for (const auto & rowParam : rowParams)
      {
        auto const & value = rowParam[rowParam.type_name()];
        if (value.is_string())
        {
          ScAddr const & addr = context->HelperFindBySystemIdtf(value);
          templParams.Add(rowParam.type_name(), addr);
        }
        else
          templParams.Add(rowParam.type_name(), ScAddr(value.get<size_t>()));
      }
    }

    if (payload.is_string())
    {
      auto * scTemplate = new ScTemplate();
      context->HelperBuildTemplate(*scTemplate, payload.get<std::string>());
      return scTemplate;
    }
    else if (payload.find("type") == payload.end())
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
    auto const & convertItemToParam = [](ScJSPayload paramItem) -> ScTemplateItemValue
    {
      std::string const & paramType = paramItem["type"];
      size_t const & paramValue = paramItem["value"].get<size_t>();

      ScTemplateItemValue param;
      if (paramItem.find("alias") == paramItem.end())
      {
        if (paramType == "type")
          param = ScType(paramValue);
        else if (paramType == "addr")
          param = ScAddr(paramValue);
      }
      else
      {
        if (paramType == "type")
          param = ScType(paramValue) >> paramItem["alias"];
        else if (paramType == "addr")
          param = ScAddr(paramValue) >> paramItem["alias"];
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
