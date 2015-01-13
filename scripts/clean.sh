#!/bin/bash

source /home/okapi/environmentSettings.bshrc

rm $OKAPI_JAVABIN/*.class
rm $OKAPI_BINDIR/*.class
rm $OKAPI_SOURCE/*.class

cd $OKAPI_SOURCE
make clean

exit 0
