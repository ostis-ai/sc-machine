/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_machine_test.hpp"

#include <sc-machine-runner/sc_machine_runner.hpp>

#include <sc-config/sc_options.hpp>

TEST_F(ScMachineTest, Run)
{
  sc_uint32 const argsNumber = 4;
  sc_char const * args[argsNumber] = {"sc-machine", "-c", SC_MACHINE_INI.c_str(), "-t"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST_F(ScMachineTest, RunWithExtensions)
{
  sc_uint32 const argsNumber = 6;
  sc_char const * args[argsNumber] = {
      "sc-machine", "-c", SC_MACHINE_INI.c_str(), "-e", SC_MACHINE_EXTENSIONS.c_str(), "-t"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST_F(ScMachineTest, RunWithStorage)
{
  sc_uint32 const argsNumber = 6;
  sc_char const * args[argsNumber] = {
      "sc-machine", "-c", SC_MACHINE_INI.c_str(), "-s", SC_MACHINE_KB_BIN.c_str(), "-t"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST_F(ScMachineTest, RunWithoutStorage)
{
  sc_uint32 const argsNumber = 4;
  sc_char const * args[argsNumber] = {"sc-machine", "-c", "", "-t"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_FAILURE);
}

TEST_F(ScMachineTest, InvalidRun)
{
  sc_uint32 const argsNumber = 1;
  sc_char const * args[argsNumber] = {"sc-machine"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_FAILURE);
}

TEST_F(ScMachineTest, RunWithVerbose)
{
  sc_uint32 const argsNumber = 5;
  sc_char const * args[argsNumber] = {"sc-machine", "-c", SC_MACHINE_INI.c_str(), "-t", "-v"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST_F(ScMachineTest, PrintHelp)
{
  sc_uint32 const argsNumber = 2;
  sc_char const * args[argsNumber] = {"sc-machine", "--help"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST_F(ScMachineTest, PrintVersion)
{
  sc_uint32 const argsNumber = 2;
  sc_char const * args[argsNumber] = {"sc-machine", "--version"};
  EXPECT_EQ(RunMachine(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}
