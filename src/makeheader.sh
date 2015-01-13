rm -rf ../src1/*;find * -type d | cpio -pdm ../src1
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
      cp $name ../src1/$name
    fi
done < headerfiles.txt

find ../src1/ -type d -empty -delete

#cd src;./makeheader.sh;cd ..;/opt/local/bin/g++ -o build/debug/customer/core_impl/gpe_impl/gperun.o -c -O0 -g3 -Wall -c -fmessage-length=0 -Wno-unused -fpermissive -fno-omit-frame-pointer -ldl "-DBUILDVERSION=\"\"" -DOPENCODE -DOS_MACOSX=1 -DComponentTest -DMULTIEDGETYPES -Isrc/third_party -Isrc/third_party/boost -Isrc/third_party/yaml/usr/local/include -Isrc/third_party/sparsehash/usr/local/include -Isrc/third_party/glog/usr/local/include -Isrc/third_party/jsoncpp/include -Isrc/third_party/redis/deps/hiredis -Isrc/third_party/zlib -Isrc/third_party/zookeeper_c/include/zookeeper -Isrc/third_party/zeromq/include -Isrc/third_party/librdkafka/include -Isrc/third_party/mpich/include -Isrc1/customer -Isrc1/core -Isrc1/core/gse -Isrc1/core/gpe -Isrc1/olgp -Isrc1/core/topology -Isrc1/utility -Isrc1/msg -Isrc1/msg/distributed -Ibuild/debug/customer/core_impl/gpe_impl -Isrc1/customer/core_impl/gpe_impl -Ibuild/debug/customer/core_impl/gpe_impl -Isrc1/customer/core_impl/gpe_impl src/customer/core_impl/gpe_impl/gperun.cpp
