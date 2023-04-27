### Config file example:
```ini
[sc-memory]
# Maximum number of segments. By default: 65536
max_loaded_segments = 1000
# Maximum number of threads that can be used to access to sc-memory. By default: 32
max_threads = 32
# Maximum number of threads that can be used in events and agents handler. By default: core number of device processor
max_events_and_agents_threads = 32

# Period (in seconds) to save sc-memory statistics
save_period = 3600
# Period (in seconds) to update sc-memory statistics
update_period = 1800

# Path to compiled knowledge base folder (kb.bin should be inside this folder)
repo_path = /path/to/kb.bin
# Path to sc-memory shared library extensions
extensions_path = /path/to/sc-machine/bin/extensions

# Maximum number of channels to split file memory into sections. By default: 1000
max_strings_channels = 1000
# Maximum file memory section size. By default: 100000
max_strings_channel_size = 100000
# Maximum size of strings that can be found by substring. By default: 1000
max_searchable_string_size = 1000
# Separators used to divide strings into tokens to find this strings by its tokens substrings. By default: " _"
# If search by substring isn't needed, set this value to "" to increase maximum performance for strings linking and searching
term_separators = " _" 

[sc-server]
# Sc-server socket data
host = 127.0.0.1
port = 8090

# Sc-server log file
log_file = /path/to/sc-machine/log/sc-server.log
# Sc-server log file
log_level = Info # it can be 'Debug' or 'Error' also

# Sc-server mode to synchronize all actions in it
sync_actions = 0
```
**Note: If you use relative paths they will be resolved based on config file location.**