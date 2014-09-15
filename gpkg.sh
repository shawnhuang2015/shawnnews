#!/bin/bash
#######################################################
# build the package                                   #
#######################################################
gsql_git_branch() {
    git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\1/'
}

# make sure ENV has setup
: ${GSQL_PROJ_HOME:?"Need to run \"source .gsql_setup_dev_env.source\""}

# also make sure the ENV are valid -- that, check if branch changed
if [ $(gsql_git_branch) != "$GSQL_BRANCH" ]; then
    echo "Seems you have switched branch. Please rerun \"source .gsql_setup_dev_env.source\""
    exit 0
fi

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


# 1. pick up the standalone pkgs (ZK, Kafka, REST) first so the customer pkg can override them
safeRunCommand "${ER_HOME}/tools/saferun.sh $@ ${ER_HOME}/buildenv/gpkg_other.conf"
safeRunCommand "${ER_HOME}/gitenv/add_repo_info.sh"
safeRunCommand "${ER_HOME}/tools/saferun.sh $@ config/pkg.config"
cd ${CWD}

# if has any option, just return
if [ "$#" -ne 0 ]; then
    exit 0
fi

# 2. package to a tar ball
tar cvfz ${GSQL_BRANCH}.tar.gz deploy_pkg
echo "${GSQL_BRANCH}.tar.gz is ready"

# 2.5 copy tar ball to mgnt_cli
cp ${GSQL_BRANCH}.tar.gz mgnt_cli/pkgs

tar cvfz ${GSQL_BRANCH}_mgntcli.tar.gz mgnt_cli
echo "${GSQL_BRANCH}_mgntcli.tar.gz is ready"

# 3. running post packaging cmd, if any
${ER_HOME}/tools/post_pkg.sh

END=$(date +%s)
DIFF=$(( $END - $START ))
echo
echo "${bldgre}$0 finished successfully in ${DIFF} seconds."
