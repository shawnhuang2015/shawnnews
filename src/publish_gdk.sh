#!/bin/bash

PACK_PROMPT="PACK    : "
UPLOAD_PROMPT="UPLAOD  : "

trim() {
    local var="$*"
    var="${var#"${var%%[![:space:]]*}"}"   # remove leading whitespace characters
    var="${var%"${var##*[![:space:]]}"}"   # remove trailing whitespace characters
    echo -n "$var"
}

declare -a OS_TYPE_VERSION
get_os_signature() {
    CENTOS_FILE="/etc/redhat-release"
    UBUNTU_FILE="/etc/issue"
    if [ -f ${CENTOS_FILE} ]; then
        version=`cat /etc/redhat-release | egrep -o '[0-9]{1}.[0-9]{1}.[0-9]{2}'`
        OS_TYPE_VERSION="centos_${version}"
    else
        if [ -f ${UBUNTU_FILE} ]; then
            version=`cat /etc/issue | egrep -o '[0-9]{2}.[0-9]{2}'`
            OS_TYPE_VERSION="ubuntu_${version}"
        else
            version=`system_profiler SPSoftwareDataType | egrep -o '[0-9]{2}.[0-9]{2}.[0-9]{1}'`
            OS_TYPE_VERSION="os_x_${version}"
        fi  
    fi  
}
get_os_signature

rm -rf ../deploy_gsdk/*
mkdir -p ../deploy_gsdk/gsdk/include
mkdir -p ../deploy_gsdk/gsdk/lib/release
mkdir -p ../deploy_gsdk/gsdk/lib/debug
mkdir -p ../deploy_gsdk/gsdk/gsql
mkdir -p ../deploy_gsdk/gsdk/bin/release
mkdir -p ../deploy_gsdk/gsdk/bin/debug
# populate include

./makeheader.sh

# populate the release and debug static libs
cd ..
find build/release/ -name "*.a" -exec cp {} deploy_gsdk/gsdk/lib/release/ \;
third_party_lib=(\
    'glog/usr/local/lib/*.a'\
    'yaml/usr/local/lib/*.a'\
    'boost/release/lib/libboost_filesystem.a'\
    'boost/release/lib/libboost_thread.a'\
    'boost/release/lib/libboost_system.a'\
    'zookeeper_c/lib/libzookeeper_mt.a'\
    'librdkafka/lib/librdkafka.a'\
    'zlib/libz.a'\
    )

for i in "${third_party_lib[@]}"
do
    echo "${PACK_PROMPT}$i"
    cp -r src/third_party/$i deploy_gsdk/gsdk/lib/release/
    cp -r src/third_party/$i deploy_gsdk/gsdk/lib/debug/
done

# add gsql
cp -RL ${GSQL_PROJ_HOME}/gsql/* deploy_gsdk/gsdk/gsql

# add binaris
cp build/release/core/gse/ids_worker ${GSQL_PKG_HOME}/bin
cp build/release/core/gse/ids_worker ${GSQL_PKG_HOME}/bin/gse_dump
cp build/release/olgp/gse/gse_loader ${GSQL_PKG_HOME}/bin
cp build/release/olgp/gse/gse_canonnical_loader ${GSQL_PKG_HOME}/bin
cp build/release/olgp/tools/graphdump ${GSQL_PKG_HOME}/bin
cp build/release/olgp/tools/batchdelta ${GSQL_PKG_HOME}/bin
common_binaries=(\
    'core/gse/ids_worker'\
    'olgp/gse/gse_loader'\
    'olgp/gse/gse_canonnical_loader'\
    'olgp/tools/graphdump'\
    'olgp/tools/batchdelta'\
    )

for i in "${common_binaries[@]}"
do
    echo "${PACK_PROMPT}$i"
    file="build/release/$i"
    if [ -f ${file} ]; then
        cp -rf ${file} deploy_gsdk/gsdk/bin/release/
    fi
    file="build/debug/$i"
    if [ -f ${file} ]; then
        cp -rf ${file} deploy_gsdk/gsdk/bin/debug/
    fi
done

# special binary: gse dump is the same as ids
current_dir=${PWD}
cd deploy_gsdk/gsdk/bin/release; ln -s ids_worker gse_dump 2>/dev/null || true ; cd ${current_dir}
cd deploy_gsdk/gsdk/bin/debug; ln -s ids_worker gse_dump 2>/dev/null || true ; cd ${current_dir}

# misc. binaries

misc_binaries=(\
    '*.sh'\
    'redis'\
    'gmalloc'\
    )

for i in "${misc_binaries[@]}"
do
    echo "${PACK_PROMPT}$i"
    file="deploy_pkg/bin/$i"
    cp -rf ${file} deploy_gsdk/gsdk/bin/release/ 2>/dev/null || true
    cp -rf ${file} deploy_gsdk/gsdk/bin/debug/ 2>/dev/null || true
done

# clean up empty dir: keep this as the last line
find deploy_gsdk/ -type d -empty -delete

cd deploy_gsdk
DATE=$(date +"%Y%m%d%H%M")
pkgname="gsdk_${OS_TYPE_VERSION}_$DATE.tar.gz"
latest_pkgname="gsdk_latest_${OS_TYPE_VERSION}_$DATE.tar.gz"
tar czf $pkgname gsdk
echo "${PACK_PROMPT}$pkgname is ready!"



# push to repos if give the option
dest='local'
rootdir='/repos'
subdir='nightly'
for i in $*; do
    if [ $i = 'mtv' ] || [ $i = 'kent' ];
    then
        dest=$i
    fi
    if [ $i = 'nightly' ] || [ $i = 'latest' ];
    then
        subdir=$i
    fi
    if [[ $i == /* ]];
    then
        rootdir=$i
    fi
done

if [ "$dest" != "local" ]; then
    connect_str="$(grep "^$dest" ../gdk/config/sdk_resource.cfg | cut -d':' -f2-)"
    connect_str=$(trim ${connect_str})
    remote_str="${connect_str}:${rootdir}/gsdk/${subdir}/${OS_TYPE_VERSION}/"
    if [ "$subdir" == 'latest' ]; then
        remote_str="${remote_str}${latest_pkgname}"
    fi
    echo "${UPLOAD_PROMPT}to $remote_str"
    curl --ftp-create-dirs -T $pkgname $remote_str
fi
