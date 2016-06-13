#!/bin/bash

source /home/okapi/environmentSettings.bshrc
echo $OKAPI_SCRIPTS
cd $OKAPI_SCRIPTS

# clean up
./clean.sh

# compile okapi C
./make_okapi.sh

# compile java src
./compile_java.sh

# compile jni
./build_jni.sh


exit 0
