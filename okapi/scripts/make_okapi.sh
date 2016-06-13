#!/bin/bash
source /home/okapi/environmentSettings.bshrc
cd $OKAPI_SOURCE
rm -rf *.o
make clean
make
cp -f ix1 ixf convert_runtime i1+ $OKAPI_BINDIR
chmod +x $OKAPI_BINDIR/*

exit 0
