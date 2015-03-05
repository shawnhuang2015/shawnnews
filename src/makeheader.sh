rm -rf ../deploy/src;rm -rf ../deploy/build;find * -type d | cpio -pdm ../deploy/src
cd ../build/release/
find . -name '*.a' | cpio -pdm ../../deploy/build/release
cd ../../src/
#find . -name '*.hpp' | cpio -pdm ../src1;find . -name '*.h' | cpio -pdm ../src1
#cloc core
#astyle --recursive ~/product/src1/core/*.hpp ~/product/src1/utility/*.hpp
# find . -name '*.h*' generete file lists

#!/bin/bash
while read line
do
    name=$line
    if [[ ${name:0:1} != '#' ]];
    then
      cp $name ../deploy/src/$name
    fi
done < headerfiles.txt

#cp -r third_party/boost ../deploy/src/third_party/
cp -r third_party/glog ../deploy/src/third_party/
cp -r third_party/sparsehash ../deploy/src/third_party/
cp -r third_party/jsoncpp ../deploy/src/third_party/
find ../deploy/ -type d -empty -delete

cd ../deploy
mkdir lib; rm -rf lib/*; find . -name "*.a" | xargs -exec cp -ft lib/

#REQUEST-10 - Open code for external customers
#cd src;./makeheader.sh;cd ..;
#/opt/local/bin/g++ -o gperun.o -c -O3 -Wall -c -fmessage-length=0 -Wno-unused -fpermissive -fno-omit-frame-pointer -ldl -ldl -DOS_MACOSX=1 -DMULTIEDGETYPES -DComponentTest -DRELEASE -Isrc/third_party -Isrc/third_party/boost -Isrc/third_party/yaml/usr/local/include -Isrc/third_party/sparsehash/usr/local/include -Isrc/third_party/glog/usr/local/include -Isrc/third_party/jsoncpp/include -Isrc/third_party/redis/deps/hiredis -Isrc/third_party/zlib -Isrc/third_party/zookeeper_c/include/zookeeper -Isrc/third_party/zeromq/include -Isrc/third_party/librdkafka/include -Isrc/third_party/mpich/include -Isrc/customer -Isrc/core -Isrc/core/gse -Isrc/core/gpe -Isrc/olgp -Isrc/core/topology -Isrc/utility -Isrc/msg -Isrc/msg/distributed -Isrc/customer/core_impl/gpe_impl -Isrc/customer/core_impl/gpe_impl gperun.cpp
#/opt/local/bin/g++ -o gperun gperun.o build/release/olgp/gpe/libolgplib.a build/release/core/gpe/libgpelib.a build/release/msg/distributed/libdistlib.a build/release/utility/gnet/libnetlib.a build/release/third_party/linenoise/liblinenoise.a build/release/third_party/murmurhash/libmurmurhash.a build/release/third_party/jsoncpp/libjson.a build/release/third_party/snappy/libsnappy.a src/third_party/yaml/usr/local/lib/libyaml-cpp.a src/third_party/boost/release/lib/libboost_date_time.a src/third_party/boost/release/lib/libboost_serialization.a src/third_party/boost/release/lib/libboost_wserialization.a src/third_party/boost/release/lib/libboost_filesystem.a src/third_party/boost/release/lib/libboost_system.a src/third_party/boost/release/lib/libboost_program_options.a src/third_party/boost/release/lib/libboost_thread.a src/third_party/glog/usr/local/lib/libglog.a src/third_party/gtest/libgtest.a src/third_party/redis/deps/hiredis/libhiredis.a src/third_party/zlib/libz.a src/third_party/zookeeper_c/lib/libzookeeper_mt.a src/third_party/zeromq/lib/libzmq.a src/third_party/librdkafka/lib/librdkafka.a src/third_party/mpich/lib/libmpi.a
