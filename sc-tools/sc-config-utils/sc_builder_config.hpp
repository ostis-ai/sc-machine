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
#include "../sc-builder/src/builder.hpp"

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
    return m_builderParams;
  }

  ~ScBuilderConfig() = default;

private:
  std::string m_groupName;
  BuilderParams m_builderParams;
  std::map<std::string, std::string> m_params;
};
