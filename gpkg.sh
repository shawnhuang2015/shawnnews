#!/bin/bash
#######################################################
# build the package                                   #
#######################################################
CWD=$(pwd)
MY_PATH="`dirname \"$0\"`"              # relative
MY_PATH="`( cd \"$MY_PATH\" && pwd )`"  # absolutized and normalized
cd ${MY_PATH}

${ER_HOME}/tools/saferun.sh "$@" "config/pkg.config"

ret_code=$?
if [ $ret_code != 0 ]; then
    printf "Error[%d] when running ${ER_HOME}/tools/saferun.sh config/pkg.config\n" $ret_code
    exit $ret_code
else
    echo "$0 finished successfully!"
fi

cd ${CWD}





