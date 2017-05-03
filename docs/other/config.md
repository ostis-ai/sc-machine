## sc-memory

**Configuration**
```bash
[memory]
max_loaded_segments = 16      # Maximum number of segments. By default: 65536

[filememory]
engine = filesystem     # used filememory plugin. Possible values: filesystem, redis

[redis]
host = 127.0.0.1      # ip-address of redis server
port = 6379       # port of redis server
timeout = 1500     # max timeout for responce from redis server in milliseconds

[python]
modules_path = ../python_modules;../python  # list of search path of python modules (default ./python)
```

## sctp-server
**Configuration**     
```bash
[Network]
Port = 56787      # port listening by sctp server

[Repo]
Path = /home/user/repo      # path to knowledge base binary
SavePeriod = 3600           # period (in seconds) to save sc-memory state

[Extensions]
Directory = /home/user/sc-machine/bin/extensions      # path to directory with sc-memory extensions (ScModules)

[Stat]
UpdatePeriod = 3600      # period (in seconds) to update sc-memory stat
Path = /home/user/stat      # path to save stat

... # configuation of sc-machine
```
When you run SCTP-server, then you need to append configuration of sc-memory (see above) to the end of configuration file for server.
