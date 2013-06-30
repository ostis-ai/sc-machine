swig -I"../../sc-memory/src" -includeall -python pysc.i
gcc -c pysc_wrap.c -I"/usr/include/python2.7" -I"../../sc-memory/src" -fPIC
ld -shared pysc_wrap.o -lsc_memory -L../../bin -o _pysc.so
cp _pysc.so ../../bin/
cp pysc.py ../../bin/
