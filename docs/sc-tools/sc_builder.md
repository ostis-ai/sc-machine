# sc-builder

Sc-builder tool allows to build knowledge base from sources. You can find it in a `bin` directory.
There are options of this tool:

<pre>
./bin/sc-builder --help

Sc-builder usage:

  --config|-c arg           Path to configuration .ini file
  --input_path|-i arg       Path to directory with sources
  --output_path|-o arg      Path to output directory (repository)
  --auto_formats|-f         Enable automatic formats info generation
  --clear                   Flag to clear sc-memory on start
  --help                    Display this message
</pre>

Example of usage:

```sh
cd sc-machine
./bin/sc-builder -c ./sc-machine.ini -i ./kb -o ./kb.bin -f --clear
```
