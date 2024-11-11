# sc-builder

Sc-builder tool allows to translate knowledge base sources into binaries. You can find it in a `bin` directory.

```sh
Usage:
  ./bin/sc-builder -i <kb-directory or repo-path> -o <binary-path> [options]

Required options:
  --input|-i                              Provide a path to directory with knowledge base sources (.scs, .gwf) or to <repo-name>.path file.
                                          This file contains knowledge base directories (or sources) that should/shouldn't be translated to binaries.
  --output|-o                             Provide a path to directory where the knowledge base binaries will be generated.

Options:
  --config|-c <config-name>.ini           Provide a path to configuration file.
                                          Configuration file can be used to set additional (optional) options for ./bin/sc-builder.
  --clear                                 Run sc-builder in the mode when it overwrites existing knowledge base binaries.
  --version                               Display version of ./bin/sc-builder.
  --help                                  Display this help message.
```

Example of usage:

```sh
cd sc-machine
./bin/sc-builder -i ./kb -o ./kb.bin --clear -c ./sc-machine.ini
```
