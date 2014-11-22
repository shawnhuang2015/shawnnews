#!/bin/sh
WORKDIR=./work
if [ ! -d "$WORKDIR" ]; then
  mkdir  "$WORKDIR"
fi

export LD_LIBRARY_PATH='/vagrant/repo2/product/bin/'
echo "LD_LIBRARY_PATH="$LD_LIBRARY_PATH

JAR=gsql.jar

java -jar "$JAR" config 

