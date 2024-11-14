# sc-builder

Sc-builder tool allows to translate knowledge base sources into binaries. You can find it in a `bin` directory.

```sh
Usage:
  ./bin/sc-builder [options]

Required options:
  --input|-i <directory>|<repo>.path       Specify the path to a directory containing knowledge base sources (.scs, .gwf) or a <repo-name>.path file.
                                           The specified file should contain knowledge base directories (or sources) that should or shouldn't be translated into binaries.
                                           This path can also be provided via the `input` option in the [sc-builder] group of the configuration file specified with --config|-c.
                                           If both options are provided, the value from --input|-i takes precedence.
  --output|-o <directory>                  Specify the path to the directory where the knowledge base binaries will be generated.
                                           This path can also be provided via the `output` option in the [sc-builder] group of the configuration file provided via --config|-c.
                                           If both options are provided, the value from --output|-o takes precedence.
  --config|-c <config-name>.ini            Specify the path to a configuration file that can set input and output paths.
                                           Use the `input` option in the [sc-builder] group for the input path, and the `output` option for the output path for ./bin/sc-builder.

Additional Options:
  --clear                                  Run sc-builder in a mode that overwrites existing knowledge base binaries.
  --version                                Display the version of ./bin/sc-builder.
  --help                                   Display this help message.
```

Example of usage:

```sh
cd sc-machine
./bin/sc-builder -i ./kb -o ./kb.bin --clear -c ./sc-machine.ini
```
