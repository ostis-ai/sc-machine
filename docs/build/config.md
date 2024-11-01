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
# !!! It is deprecated option in sc-machine 0.9.0.
save_period = 3600 
# It is equal to `save_period`. By default, it is 3600.
dump_memory_period = 3600
# Boolean indicating to enable sc-memory dump.
dump_memory = true
# Period (in seconds) to update sc-memory statistics. By default, it is 1800.
# !!! It is deprecated option in sc-machine 0.9.0.
update_period = 1800
# It is equal to `update_period`. By default, it is 1800.
dump_memory_statistics_period = 1800
# Boolean indicating to enable sc-memory statistics dump.
dump_memory_statistics = true

# Path to compiled knowledge base folder (kb.bin should be inside this folder). By default, it is empty.
binaries = /path/to/kb.bin
# Path to sc-memory shared library extensions.
extensions_path = /path/to/sc-machine/bin/extensions

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

# Sc-server mode to call parallely all input actions. By default, it is true.
parallel_actions = true

# Sc-server log type. It can be `File` or `Console`.
log_type = File
# Sc-server log file.
log_file = /path/to/sc-machine/log/sc-server.log
# Sc-server log level. # It can be `Debug`, `Info`, `Warning` or `Error` also.
log_level = Info

[sc-builder]
# Path with kb sources for building into sc-memory.
input_path = kb

# File to dump sc-builder errors.
log_file = /path/to/sc-machine/log/sc_builder.log
```
**Note: If you use relative paths they will be resolved based on config file location.**