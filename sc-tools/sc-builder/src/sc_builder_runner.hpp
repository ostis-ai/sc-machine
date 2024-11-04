/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include <sc-core/sc_types.h>
}

/**
 * @brief Runs the sc-builder with the specified command-line arguments.
 *
 * This function initializes the sc-builder based on the provided command-line options,
 * manages configuration loading, and executes the building process for knowledge base binaries.
 * It also supports displaying help information and version details.
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
 * 4. Checks for required input and output paths specified by `--input` or `-i` and `--output` or `-o`.
 * 5. Optionally reads a configuration file specified by `--config` or `-c`.
 * 6. Constructs memory parameters and configuration for the sc-memory using provided options and defaults.
 * 7. Initializes a `Builder` object and runs it with the constructed parameters.
 *
 * Error handling:
 * - If required options are missing, appropriate error messages are printed to standard output.
 * - The function will return EXIT_FAILURE if any critical errors occur during initialization or if required options are
 * not provided.
 * - Exceptions thrown by utility functions are caught, logged, and result in an EXIT_FAILURE return value.
 */
sc_int RunBuilder(sc_int argc, sc_char * argv[]);
