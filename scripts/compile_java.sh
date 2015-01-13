#!/bin/bash

source /home/okapi/environmentSettings.bshrc
cd $OKAPI_JAVASRC
echo $OKAPI_JAVASRC

javac -d $OKAPI_JAVABIN *.java

exit 0
