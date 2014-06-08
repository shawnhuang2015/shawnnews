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

START=$(date +%s)
CWD=$(pwd)
MY_PATH="`dirname \"$0\"`"              # relative
MY_PATH="`( cd \"$MY_PATH\" && pwd )`"  # absolutized and normalized
cd ${MY_PATH}
# pick up the standalone pkgs (ZK, Kafka, REST) so the customer pkg can override them
safeRunCommand "${ER_HOME}/tools/saferun.sh $@ ${ER_HOME}/buildenv/gpkg_other.conf"
safeRunCommand "${ER_HOME}/tools/saferun.sh $@ config/pkg.config"
cd ${CWD}

tar cvfz ${GSQL_BRANCH}.tar.gz deploy_pkg
echo "${GSQL_BRANCH}.tar.gz is ready"

END=$(date +%s)
DIFF=$(( $END - $START ))
echo
echo "${bldgre}$0 finished successfully in ${DIFF} seconds."

