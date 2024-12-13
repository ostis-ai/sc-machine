/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/test/sc_test.hpp>

#include <filesystem>

class ScDictionaryFSMemoryTest : public testing::Test
{
public:
  static inline sc_char SC_DICTIONARY_FS_MEMORY_PATH[10] = "fs-memory";
  static inline sc_char SC_DICTIONARY_FS_MEMORY_STRINGS_PATH[24] = "fs-memory/strings1.scdb";

protected:
  void SetUp() override {}

  void TearDown() override
  {
    std::filesystem::remove_all(SC_DICTIONARY_FS_MEMORY_PATH);
  }
};
