#!/bin/tcsh
#include <jni.h>
cd $OKAPI_SOURCE
rm -rf *.o
make
cp -f ix1 ixf convert_runtime i1+ $OKAPI_BINDIR
