#!/bin/bash
source /home/okapi/okapi-2.4.1/environmentSettings.bshrc
cd $OKAPI_SOURCE
rm -rf *.o
make
cp -f ix1 ixf convert_runtime i1+ $OKAPI_BINDIR
