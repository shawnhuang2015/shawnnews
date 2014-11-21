#!/bin/sh
WORKDIR=./work
if [ ! -d "$WORKDIR" ]; then
  mkdir  "$WORKDIR"
fi
OUTPUT=./output
if [ ! -d "$OUTPUT" ]; then
  mkdir  "$OUTPUT"
fi

JAR=gsql.jar

java -jar "$JAR" config 

