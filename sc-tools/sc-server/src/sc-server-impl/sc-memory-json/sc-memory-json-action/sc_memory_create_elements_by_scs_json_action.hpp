/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */


#pragma once

#include "sc_memory_json_action.hpp"

#include "sc-memory/sc_link.hpp"

#include "sc-memory/sc_scs_helper.hpp"

class DummyFileInterface : public SCsFileInterface
{
protected:
  ScStreamPtr GetFileContent(std::string const & fileURL) override
  {
    return {};
  }
};

class ScMemoryCreateElementsByScsJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(
      ScMemoryContext * context, ScMemoryJsonPayload requestPayload, ScMemoryJsonPayload & errorsPayload) override
  {
    ScMemoryJsonPayload responsePayload = ScMemoryJsonPayload::array({});

    sc_uint32 i = 0;
    for (auto & atom : requestPayload)
    {
      std::string const & scs = atom.get<std::string>();
      if (m_helper == nullptr)
        m_helper = new SCsHelper{*context, std::make_shared<DummyFileInterface>()};

      sc_bool textGenResult = SC_FALSE;

      try
      {
         m_helper->GenerateBySCsTextLazy(scs);
         textGenResult = SC_TRUE;
      }
      catch (utils::ScException const & e)
      {
        SC_LOG_ERROR(e.Message());
        errorsPayload.push_back({{"ref", i}, {"message", e.Message()}});
      }

      responsePayload.push_back(textGenResult);
      ++i;
    }

    if (responsePayload.is_null())
      return "{}"_json;

    return responsePayload;
  }

  ~ScMemoryCreateElementsByScsJsonAction() override
  {
    delete m_helper;
  }

private:
  SCsHelper * m_helper;
};
