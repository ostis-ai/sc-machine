/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <vector>
#include <map>

extern "C"
{
#include <sc-core/sc_memory_params.h>
}

class ScOptions;
class ScParams;
class ScConfig;
class ScMemoryConfig;

class ScParams
{
public:
  friend class ScMemoryConfig;

  explicit ScParams(ScOptions const & options, std::vector<std::vector<std::string>> const & keysSet);

  ScParams();

  ScParams(ScParams const & object) = default;
  ScParams & operator=(ScParams const & other) = default;

  void Insert(std::pair<std::string, std::string> const & pair);

  template <typename TContentType>
  TContentType const & Get(std::string const & key) const;

  template <typename TContentType>
  TContentType const & Get(std::string const & key, TContentType const & defaultValue) const;

  sc_bool Has(std::string const & key) const;

private:
  std::map<std::string, std::string> m_params;
};
