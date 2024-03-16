# sc-machine

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
---

sc-machine is a sc-memory with opportunity to add modules consistent with the general sc-machine interface. All such 
modules are named extensions, must be shared libraries (with file extension `.so`) and stored in common directory 
(by default it is `./bin/extensions`). By default, sc-machine contains sc-server that allows to communicate with 
sc-memory through network. You can find `sc-machine` in a `bin` directory.

There are options of sc-machine:

<pre>
./bin/sc-machine --help

sc-machine usage:

  --config|-c               Path to configuration file
  --extensions_path|-e      Path to directory with sc-memory extensions
  --repo_path|-r            Path to kb.bin folder
  --clear                   Flag to clear sc-memory state on initialize
  --verbose|-v              Flag to don't save sc-memory state on shutdown
  --test|-t                 Flag to test sc-machine (sc-machine with this option runs and stops)
  --help                    Display this message
</pre>

Example of usage:

```sh
cd sc-machine
./bin/sc-machine -c ./sc-machine.ini
```
