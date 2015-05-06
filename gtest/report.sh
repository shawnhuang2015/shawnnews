#!/bin/bash

for file in ./diff/*/regress*/*.diff; do
  echo "=========================================="
  echo cat diffs: $file
  echo "=========================================="
  cat $file
  file2=${file/diff/output}
  echo "=========================================="
  echo cat logs:  ${file2/diff/log}
  echo "=========================================="
  cat ${file2/diff/log}
done
