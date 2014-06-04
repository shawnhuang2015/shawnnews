source src/er/buildenv/gbuildenv.sh  proj.config

# set proj and pkg env
export GSQL_PROJ_HOME=$(pwd)
export GSQL_PKG_HOME="${GSQL_PROJ_HOME}/deploy_pkg"
mkdir -p $GSQL_PKG_HOME
export GSQL_PKG_TMP="${GSQL_PROJ_HOME}/tmp"
mkdir -p $GSQL_PKG_TMP

# set git env
cd $ER_HOME/gitenv
sh install.sh
cd $GSQL_PROJ_HOME
