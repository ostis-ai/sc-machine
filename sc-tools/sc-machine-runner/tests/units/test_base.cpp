/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include "test_defines.hpp"

#include <sc-machine-runner/sc_machine_runner.hpp>

#include <sc-config/sc_options.hpp>

TEST(ScMachine, Run)
{
  sc_uint32 const argsNumber = 4;
  sc_char const * args[argsNumber] = {"sc-machine", "-c", SC_MACHINE_INI, "-t"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScMachine, RunWithExtensions)
{
  sc_uint32 const argsNumber = 6;
  sc_char const * args[argsNumber] = {"sc-machine", "-c", SC_MACHINE_INI, "-e", SC_MACHINE_EXTENSIONS, "-t"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScMachine, RunWithBinaries)
{
  sc_uint32 const argsNumber = 6;
  sc_char const * args[argsNumber] = {"sc-machine", "-c", SC_MACHINE_INI, "-b", SC_MACHINE_REPO_PATH, "-t"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScMachine, RunWithoutBinaries)
{
  sc_uint32 const argsNumber = 4;
  sc_char const * args[argsNumber] = {"sc-machine", "-c", "", "-t"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_FAILURE);
}

TEST(ScMachine, InvalidRun)
{
  sc_uint32 const argsNumber = 1;
  sc_char const * args[argsNumber] = {"sc-machine"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_FAILURE);
}

TEST(ScMachine, RunWithVerbose)
{
  sc_uint32 const argsNumber = 5;
  sc_char const * args[argsNumber] = {"sc-machine", "-c", SC_MACHINE_INI, "-t", "-v"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScMachine, PrintHelp)
{
  sc_uint32 const argsNumber = 2;
  sc_char const * args[argsNumber] = {"sc-machine", "--help"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScMachine, PrintVersion)
{
  sc_uint32 const argsNumber = 2;
  sc_char const * args[argsNumber] = {"sc-machine", "--version"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}
