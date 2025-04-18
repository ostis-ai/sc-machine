/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_params_h_
#define _sc_memory_params_h_

#include "sc-core/sc_types.h"
#include "sc-core/sc_memory_version.h"

#define DEFAULT_MAX_LOADED_SEGMENTS 1000
#define DEFAULT_LIMIT_MAX_THREADS_BY_MAX_PHYSICAL_CORES SC_TRUE
#define DEFAULT_MAX_EVENTS_AND_AGENTS_THREADS 32
#define DEFAULT_MIN_EVENTS_AND_AGENTS_THREADS 1
#define DEFAULT_DUMP_MEMORY SC_TRUE
#define DEFAULT_DUMP_MEMORY_PERIOD 32000
#define DEFAULT_DUMP_MEMORY_STATISTICS SC_TRUE
#define DEFAULT_DUMP_MEMORY_STATISTICS_PERIOD 16000
#define DEFAULT_LOG_TYPE "Console"
#define DEFAULT_LOG_FILE ""
#define DEFAULT_LOG_LEVEL "Info"
#define DEFAULT_MAX_STRINGS_CHANNELS 10000
#define DEFAULT_MAX_STRINGS_CHANNEL_SIZE 100000
#define DEFAULT_MAX_SEARCHABLE_STRING_SIZE 1000
#define DEFAULT_TERM_SEPARATORS " _"
#define DEFAULT_SEARCH_BY_SUBSTRING SC_TRUE

/*! Structure representing parameters for configuring the sc-memory.
 * @note This structure holds various configuration parameters that control the behavior of the sc-memory.
 */
typedef struct
{
  sc_version version;  ///< Version information for the sc-machine.

  ///< Boolean indicating whether to clear existing data during initialization. By default, it is SC_FALSE.
  sc_bool clear;
  sc_char const * storage;                  ///< Path to the binaries directory.
  sc_char const ** extensions_directories;  ///< Array of extensions directories.
  sc_uint32 extensions_directories_count;   ///< Size of extensions directories array.
  sc_char const ** enabled_extensions;      ///< Array of enabled extensions.

  sc_uint32 max_loaded_segments;  ///< Maximum number of loaded segments.

  ///< Boolean indicating whether sc-memory limit `max_events_and_agents_threads` by maximum physical core number.
  sc_bool limit_max_threads_by_max_physical_cores;
  sc_uint32 max_events_and_agents_threads;  ///< Maximum number of threads for events and agents processing.

  ///< Boolean indicating whether automatic saving of sc-memory state. By default, it is SC_TRUE.
  sc_bool dump_memory;
  sc_uint32 dump_memory_period;  ///< Period (in seconds) for automatic saving of sc-memory state.

  ///< Boolean indicating whether automatic dumping statistics of sc-memory state. By default, it is SC_TRUE.
  sc_bool dump_memory_statistics;
  sc_uint32 dump_memory_statistics_period;  ///< Period (in seconds) for dumping statistics of sc-memory state.

  sc_char const * log_type;   ///< Type of logging (e.g., "Console", "File").
  sc_char const * log_file;   ///< Path to the log file (if log_type is "File").
  sc_char const * log_level;  ///< Log level (e.g., "Error", "Warning", "Info", "Debug").

  sc_char const * init_memory_generated_structure;  ///< Initial sc-memory generated structure system identifier.
  sc_bool init_memory_generated_upload;  ///< Boolean indicating whether to upload the initial generated structure.

  sc_bool user_mode;  ///< Boolean indicating whether the sc-memory is in user mode.

  sc_uint16 max_strings_channels;        ///< Maximum number of string channels.
  sc_uint32 max_strings_channel_size;    ///< Maximum size of a string channel.
  sc_uint32 max_searchable_string_size;  ///< Maximum size of a searchable string.
  sc_char const * term_separators;       ///< String containing term separators used in string operations.
  sc_bool search_by_substring;           ///< Boolean indicating whether to allow searching by substring.
} sc_memory_params;

_SC_EXTERN void sc_memory_params_clear(sc_memory_params * params);

#endif
