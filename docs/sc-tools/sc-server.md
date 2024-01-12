# SC-server

SC-server tool allows to communicate with sc-memory through network. You can find it in a `bin` directory.
There are options of this tool:

<pre>
./bin/sc-server --help

SC-server usage:

  --config|-c               Path to configuration file
  --host|-h                 Sc-server host name, ip-address
  --port|-p                 Sc-server port
  --extensions_path|-e      Path to directory with sc-memory extensions
  --repo_path|-r            Path to kb.bin folder
  --verbose|-v              Flag to don't save sc-memory state on exit
  --clear                   Flag to clear sc-memory on start
  --help                    Display this message
  --test|-t                 Flag to test sc-server, run and stop it
</pre>

Example of usage:

```sh
cd sc-machine
./bin/sc-server -c ./sc-machine.ini
```
