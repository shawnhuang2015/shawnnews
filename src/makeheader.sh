#!/bin/bash
DEPLOY_GSDK_INCLUDE="../deploy_gsdk/gsdk/include"
PACK_PROMPT="PACK    : "

# populate include
find * -type d | cpio -pdm ${DEPLOY_GSDK_INCLUDE}

while read line
do
    name=$line
    if [[ ${name:0:1} != '#' ]];
    then
        cp $name ${DEPLOY_GSDK_INCLUDE}/$name
    fi
done < headerfiles.txt

third_party_array=(\
    'glog/usr/local/include'\
    'sparsehash/usr/local/include'\
    'jsoncpp'\
    'redis/deps/hiredis'
)

for i in "${third_party_array[@]}"
do
    echo "${PACK_PROMPT}$i"
    mkdir -p ${DEPLOY_GSDK_INCLUDE}/third_party/$i; cp -r third_party/$i/* ${DEPLOY_GSDK_INCLUDE}/third_party/$i/
done
cp -r third_party/boost/boost ${DEPLOY_GSDK_INCLUDE}/third_party/

