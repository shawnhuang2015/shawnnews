#!/bin/sh
WORKDIR=./work
if [ ! -d "$WORKDIR" ]; then
  mkdir  "$WORKDIR"
fi

LOGDIR="$WORKDIR/log"

if [ ! -d "$LOGDIR" ]; then
  mkdir  "$LOGDIR"
fi

export LD_LIBRARY_PATH='./TokenUDF'


JAR=gsql.jar

java -jar "$JAR" config 

