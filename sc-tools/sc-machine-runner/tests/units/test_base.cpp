/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include "../test_defines.hpp"

#include "../../src/sc_machine_setup.hpp"

#include "sc_options.hpp"

TEST(ScMachine, RunMain)
{
  sc_uint32 const argsNumber = 4;
  sc_char const * args[argsNumber] = {"sc-machine", "-c", SC_MACHINE_INI, "-t"};
  EXPECT_EQ(BuildAndRunMachine(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScMachine, InvalidRunMain)
{
  sc_uint32 const argsNumber = 1;
  sc_char const * args[argsNumber] = {"sc-machine"};
  EXPECT_EQ(BuildAndRunMachine(argsNumber, (sc_char **)args), EXIT_FAILURE);
}
