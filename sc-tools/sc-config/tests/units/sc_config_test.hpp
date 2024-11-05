/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/test/sc_test.hpp>

class ScConfigTest : public testing::Test
{
public:
  static inline std::string const & SC_CONFIG_INI = "../sc-machine-test.ini";
  static inline std::string const & SC_CONFIGS = "../configs";

protected:
  void SetUp() override {}

  void TearDown() override {}
};
