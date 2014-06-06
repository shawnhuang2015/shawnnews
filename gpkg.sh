#!/bin/bash
#######################################################
# build the package                                   #
#######################################################

# execution function
safeRunCommand() {
    typeset cmnd="$*"
    typeset ret_code
    now=$(date +"%T")
    echo "${bldred}[${now}]Running: ${bldblu}${cmnd}${txtrst}"
    eval $cmnd
    ret_code=$?
    if [ $ret_code != 0 ]; then
        printf "${bldred}Error[%d]: ${bldblu}${cmnd}${txtrst}\n" $ret_code
        exit $ret_code
    fi
}

CWD=$(pwd)
MY_PATH="`dirname \"$0\"`"              # relative
MY_PATH="`( cd \"$MY_PATH\" && pwd )`"  # absolutized and normalized
cd ${MY_PATH}
safeRunCommand "${ER_HOME}/tools/saferun.sh $@ config/pkg.config"
safeRunCommand "${ER_HOME}/tools/saferun.sh $@ ${ER_HOME}/buildenv/gpkg_other.conf"

cd ${CWD}





