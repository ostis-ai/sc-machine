#!/bin/bash
export LD_LIBRARY_PATH=../bin
../bin/sc-server -e ../bin/extensions -r ../kb.bin -i ../bin/config.ini
