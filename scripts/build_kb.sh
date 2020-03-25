#!/bin/bash
export LD_LIBRARY_PATH=../bin
../bin/sc-builder -i ../kb -o ../kb.bin -c -s ../bin/config.ini -f -e ../bin/extensions
