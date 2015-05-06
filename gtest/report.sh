#!/bin/sh

for file in ./diff/*/regress*/*.diff; do
  file2=${file/diff/output}
  echo cat ${file2/diff/log}
  cat ${file2/diff/log}
done
