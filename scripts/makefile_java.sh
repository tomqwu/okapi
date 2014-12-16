#!/bin/bash
source /home/okapi/okapi/environmentSettings.bshrc
cd $OKAPI_BINDIR
echo $OKAPI_BINDIR
javac -cp . relex.java
javac -Xlint -cp . BrowserLauncher.java
javac -Xlint -cp . Notepad.java
javac -Xlint -cp . demo.java
javac -Xlint -cp . build_index.java
javac -Xlint -cp . javokapi.java
javac -Xlint -cp . priority_query.java
javac -Xlint -cp . query.java
javac -Xlint -cp . query_list.java
javac -Xlint -cp . searching.java
