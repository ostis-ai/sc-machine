# SC-server

SC-server allows to communicate with sc-memory through network. You can find it in a `bin` directory.
There are options of this tool:

!!! warning
    Now sc-server is an extension and all extensions are loaded by executable `sc-machine`. The executable
    `sc-server` is deprecated in sc-machine 0.10.0. It will be removed in sc-machine 0.11.0. Use executable `sc-machine` 
    instead.

<pre>
./bin/sc-server --help

SC-server usage:

  --config|-c               Path to configuration file
  --host|-h                 Sc-server host name, ip-address
  --port|-p                 Sc-server port
  --extensions_path|-e      Path to directory with sc-memory extensions
  --repo_path|-r            Path to kb.bin folder
  --clear                   Flag to clear sc-memory state on initialize
  --verbose|-v              Flag to don't save sc-memory state on shutdown
  --test|-t                 Flag to test sc-server, run and stop it
  --help                    Display this message
</pre>

Example of usage:

```sh
cd sc-machine
./bin/sc-server -c ./sc-machine.ini
```
