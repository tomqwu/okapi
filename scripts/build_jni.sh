#!/bin/bash

source /home/okapi/environmentSettings.bshrc
cd $OKAPI_JAVASRC
javac -d $OKAPI_JAVABIN relex.java

cd $OKAPI_JAVABIN
javah -d $OKAPI_BINDIR -jni relex

cd $OKAPI_SOURCE
gcc -m32 -fPIC -DMMPUT -DHOLD_ATOMIC_MAPS=0 -DHOLD_DOCLENS=1 -I. -L.  -c  -o relex.o relex.c
gcc -m32 -shared -o librelex.so *.o
#cp -f relex.* relex.o librelex.so $OKAPI_BINDIR

exit 0
