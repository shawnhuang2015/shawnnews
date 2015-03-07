#!/bin/bash
rm -rf ../deploy/*
mkdir -p ../deploy/gsdk/include
mkdir -p ../deploy/gsdk/lib/release
mkdir -p ../deploy/gsdk/lib/debug

# populate include
find * -type d | cpio -pdm ../deploy/gsdk/include

while read line
do
    name=$line
    if [[ ${name:0:1} != '#' ]];
    then
        cp $name ../deploy/gsdk/include/$name
    fi
done < headerfiles.txt

third_party_array=(\
    'glog/usr/local/include'\
    'sparsehash/usr/local/include'\
    'jsoncpp'
'redis/deps/hiredis'
)

for i in "${third_party_array[@]}"
do
    echo "key  : $i"
    mkdir -p ../deploy/gsdk/include/third_party/$i; cp -r third_party/$i/* ../deploy/gsdk/include/third_party/$i/
done
cp -r third_party/boost/boost ../deploy/gsdk/include/third_party/

# populate the release and debug static libs
cd ..
find build/release/ -name "*.a" -exec cp {} deploy/gsdk/lib/release/ \;
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
    echo "key  : $i"
    cp -r src/third_party/$i deploy/gsdk/lib/release/
    cp -r src/third_party/$i deploy/gsdk/lib/debug/
done

find deploy/ -type d -empty -delete

