### Config file example:
```ini
[sc-memory]
# Maximum number of segments. By default: 65536
max_loaded_segments = 1000
# Maximum number of threads that can be used to access to sc-memory
max_threads = 32
# Maximum number of threads that can be used in events and agents handler
max_events_and_agents_threads = 32

# Period (in seconds) to save sc-memory stat
save_period = 3600
# Period (in seconds) to update sc-memory stat
update_period = 1800

# Path to compiled knowledge base folder (kb.bin should be inside this folder)
repo_path = /path/to/kb.bin
# Path to sc-memory shared library extensions
extensions_path = /path/to/sc-machine/bin/extensions

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