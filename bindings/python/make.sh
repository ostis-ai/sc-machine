swig -I"../../sc-memory/src" -includeall -python sc-memory.i
gcc -c sc-memory_wrap.c -I"/usr/include/python2.7" -I"../../sc-memory/src"
ld -shared sc-memory_wrap.o -lsc_memory -L../../sc-memory/src -o _sc_memory.so
cp _sc_memory.so ../../bin/
cp sc_memory.py ../../bin/
