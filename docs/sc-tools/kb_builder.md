# Builder

Builder tool allow to build knowledge base from a sources. You can find it in a `bin` directory.
There are options of this tool:

<pre>
sc-builder --help

Builder usage:
  --help                      Display this message
  --version                   Displays version number
  -i [ --input-path ] arg     Path to directory with sources
  -o [ --output-path ] arg    Path to output directory (repository)
  -c [ --clear-output ]       Clear output directory (repository) before build
  -s [ --settings ] arg       Path to configuration file for sc-memory
  -f [ --auto-formats ]       Enable automatic formats info generation
</pre>

Example of usage:

```
./bin/sc-builder -i ./kb -o ./kb.bin -c -s ./bin/config.ini -f -e ./bin/extension
```

## Extensions

There is a possibility to specify which extensions will be runned during a knowledge base building.
You can specify that with parameter `--enabled-ext`. Value of this parameter is a path to file
that contains list of allowed extensions. Example of file content:

```
sc-utils
```

In that case just `sc-utils` extensions will be loaded during knowledge base build process.

Example of usage:

```
./bin/sc-builder -i ./kb -o ./kb.bin -c -s ./bin/config.ini -f -e ./bin/extension --enabled-ext ./config/build_ext.list
```



