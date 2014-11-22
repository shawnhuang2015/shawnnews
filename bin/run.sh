#!/bin/sh
WORKDIR=./work
if [ ! -d "$WORKDIR" ]; then
  mkdir  "$WORKDIR"
fi

JAR=gsql.jar

java -jar "$JAR" config 

