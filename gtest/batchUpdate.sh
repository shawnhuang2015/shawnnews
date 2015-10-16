#!/bin/bash

# get options -n
# if -n is not specify, will show user the diff and ask whether to copy
getopts "n" FLAG
shift $((OPTIND-1))

# execut gtest
cd $PRODUCT/gtest;./gtest gsql.sh $@ > /tmp/a
cat /tmp/a > /tmp/gtest_result

# get commands
grep "Update\: cp"  /tmp/a > /tmp/b
grep "Check\: less"  /tmp/a | awk '{ print "cat " $3 }' > /tmp/c
sed 's/Update\://g' /tmp/b > /tmp/a

while read -r line && read -r diff <&3
do
  if [[ $FLAG == "n" ]]; then
    yes="y"
  else
    echo $diff
    eval $diff
    # ask for confirm
    echo -n "Do you want to update? [Y/N]"
    read -n 1 yes </dev/tty; echo
  fi 

  if [[ $yes == Y || $yes == y ]]; then
    # execute update
    echo $line
    eval $line
  fi
  echo -e "\n\n"
done < "/tmp/a" 3<"/tmp/c"

rm /tmp/a
rm /tmp/b
rm /tmp/c

echo "Done."
