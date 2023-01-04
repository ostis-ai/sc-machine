/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <utility>
#include <map>

#include "sc_memory_config.hpp"

#include "sc-config/sc_config.hpp"

#include "sc-core/sc_memory_params.h"

#define DEFAULT_RESULT_STRUCTURE_MAIN_IDTF ""
#define DEFAULT_RESULT_STRUCTURE_UPLOAD SC_FALSE

class ScBuilderConfig
{
public:
  explicit ScBuilderConfig(ScConfig const & config, BuilderParams params, std::string groupName = "sc-builder")
    : m_groupName(std::move(groupName))
    , m_builderParams(std::move(params))
  {
    if (config.IsValid())
    {
      ScConfigGroup group = config[m_groupName];
      for (auto const & key : *group)
      {
        std::string const & value = group[key];
        std::stringstream stream;

        m_params.insert({key, value});
      }
    }
  }

  BuilderParams GetParams()
  {
    m_builderParams.m_resultStructureUpload =
        m_params.count("result_structure_upload")
            ? (m_params.at("result_structure_upload") == "true" ? SC_TRUE : SC_FALSE)
            : DEFAULT_RESULT_STRUCTURE_UPLOAD;
    m_builderParams.m_resultStructureSystemIdtf = m_params.count("result_structure_system_idtf")
                                                      ? m_params.at("result_structure_system_idtf").c_str()
                                                      : DEFAULT_RESULT_STRUCTURE_MAIN_IDTF;

    return m_builderParams;
  }

  ~ScBuilderConfig() = default;

private:
  std::string m_groupName;
  BuilderParams m_builderParams;
  std::map<std::string, std::string> m_params;
};
