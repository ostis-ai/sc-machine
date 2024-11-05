/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_type.hpp>

/*!
 * @brief Runs the sc-machine with the specified command-line arguments.
 *
 * This function initializes the sc-machine based on the provided command-line options,
 * handles configuration loading, and manages the machine's lifecycle. It also supports
 * displaying help information and version details.
 *
 * @param argc A number of command-line arguments.
 * @param argv An array of command-line argument strings.
 *
 * @return Returns EXIT_SUCCESS (0) if the operation completes successfully, 
 *         or EXIT_FAILURE (1) if there is an error in processing the command-line options
 *         or during initialization.
 *
 * The function performs the following steps:
 * 1. Parses command-line options using the `ScOptions` class.
 * 2. Displays help information if the `--help` option is specified.
 * 3. Displays version information if the `--version` option is specified.
 * 4. Checks for a required configuration file specified by `--config` or `-c`.
 * 5. Initializes the sc-memory with parameters from the configuration file and command-line options.
 * 6. Sets up signal handling to gracefully shut down the machine when needed.
 * 7. Enters a loop that keeps the machine running until a termination signal is received.
 * 8. Cleans up and shuts down the sc-memory system before exiting.
 *
 * Error Handling:
 * - If required options are missing, appropriate error messages are printed to standard output.
 * - The function will return EXIT_FAILURE if any critical errors occur during initialization or if required options are not provided.
 * - Exceptions thrown by utility functions are caught, logged, and result in an EXIT_FAILURE return value.
 */
sc_int RunMachine(sc_int argc, sc_char * argv[]);
