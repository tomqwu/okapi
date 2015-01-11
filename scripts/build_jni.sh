#!/bin/bash
source /home/okapi/environmentSettings.bshrc
echo $OKAPI_SOURCE
cd $OKAPI_SOURCE
javac relex.java
javah -jni relex
gcc -m32 -fPIC -DMMPUT -DHOLD_ATOMIC_MAPS=0 -DHOLD_DOCLENS=1 -I. -L.  -c  -o relex.o relex.c
gcc -m32 -shared -o librelex.so *.o
cp -f relex.* librelex.so $OKAPI_BINDIR
