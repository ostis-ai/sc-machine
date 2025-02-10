### Config file example

```ini
[sc-memory]
# Maximum number of segments. By default, it is 1000.
# Remember, that one sc-segment size is 3932144 bytes. 1000 segments size is 4 GB.
max_loaded_segments = 1000

# If it is equal to `true` then sc-memory use minimum between physical cores number and `max_events_and_agents_threads`.
limit_max_threads_by_max_physical_cores = true
# Maximum number of threads that can be used in events and agents handler. By default, it is 32 if 
`limit_max_threads_by_max_physical_cores` is `true` or otherwise it is core number of device processor.
max_events_and_agents_threads = 32

# Period (in seconds) to save sc-memory statistics. By default, it is 3600.
dump_memory_period = 3600
# Boolean indicating to enable sc-memory dump.
dump_memory = true
# Period (in seconds) to update sc-memory statistics. By default, it is 1800.
dump_memory_statistics_period = 1800
# Boolean indicating to enable sc-memory statistics dump.
dump_memory_statistics = true

# Path to folder with compiled knowledge base binaries. By default, it is empty.
storage = /path/to/kb.bin
# List of paths to directories with sc-memory shared library extensions separated by semicolon.
extensions = /path/to/sc-machine/bin/extensions_1;/path/to/sc-machine/bin/extensions_2;...

# Sc-memory log type. It can be `File` or `Console`.
log_type = File
# Sc-memory log file.
log_file = /path/to/sc-machine/log/sc-server.log
# Sc-memory log level. # It can be `Debug`, `Info`, `Warning` or `Error` also.
log_level = Info

# Boolean indicating to upload all sc-element into one common sc-structure with system identifier `result_structure`.
init_memory_generated_upload = false
init_memory_generated_structure = result_structure

# Maximum number of channels to split file memory into sections. By default, it is 1000.
max_strings_channels = 1000
# Maximum file memory section size. By default, it is 100000.
max_strings_channel_size = 100000
# Maximum size of strings that can be found by substring. By default, it is 1000.
max_searchable_string_size = 1000
# Separators used to divide strings into tokens to find this strings by its tokens substrings. By default, it is " _".
# If search by substring isn't needed, set this value to "" to increase maximum performance for strings linking and searching.
term_separators = " _" 
# If search by substring isn't needed, set this value to "false" to increase maximum performance for strings linking.
search_by_substring = true

[sc-server]
# Sc-server socket data.
host = 127.0.0.1
port = 8090

# Sc-server mode to call parallel all input actions. By default, it is true.
parallel_actions = true

# Sc-server log type. It can be `File` or `Console`.
log_type = File
# Sc-server log file.
log_file = /path/to/sc-machine/log/sc-server.log
# Sc-server log level. # It can be `Debug`, `Info`, `Warning` or `Error` also.
log_level = Info

[sc-builder]
# Path to directory with knowledge base sources (.scs, .gwf) or to <repo-name>.path file
input = kb
# Path to folder with compiled knowledge base binaries. By default, it is empty.
output = kb.bin
```
!!! Note
    If you use relative paths they will be resolved based on config file location.
