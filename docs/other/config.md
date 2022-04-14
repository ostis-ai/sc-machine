### Config file example:
```ini
[Network]
#port for sctp connection
Port = 55770
[Repo]
# directory where KB build error log file will be stored
Logfile = ./
# path to compiled knowledge base folder (kb.bin should be inside this folder)
Path = ../../
# time before KB binary dump
SavePeriod = 3600 
[Extensions]
# path to sc-memory shared library extensions
Directory = ../bin/extensions
[Stat]
# period (in seconds) to update sc-memory stat
UpdatePeriod = 1800
# path to save stat
Path = /tmp/sctp_stat

[memory]
# Maximum number of segments. By default: 65536
max_loaded_segments = 1000

[python]
# list of search path of python modules separated by a semicolon (default: sc-machine/sc-kpm/sc-python/services)
modules_path = ../python_modules;../python  

[debug]
# debug mode enabled, more log messages (True|False)
is_debug = True
```
**Note: If you use relative paths they will be resolved based on config file location.**