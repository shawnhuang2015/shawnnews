#!/bin/bash

BASELINES=$(find $PRODUCT/gtest/base_line/gsql/regress5/*)
FLAG=0
for file in $BASELINES
do
  IFS="$(printf '\n')"
  echo -e "#include \"querydispatcher.hpp\"\n" > /tmp/baseline.cpp

  # parsing base line, and extract useful content
  while read -r line
  do
    if [[ $line == \#include* ]]
    then
      FLAG=1
    fi

    if [[ $line == "Query "*" has been added"* ]]
    then
      QNAME=$(echo $line | awk '{ print $2 }')
      FLAG=0
    fi

    if [ $FLAG -eq 1 ]
    then
      echo $line >> /tmp/baseline.cpp
    fi

  done < $file

  # add more info
  echo -e "namespace UDIMPL {
  bool QueryDispatcher::RunQuery(){
  if (QueryName == \""$QNAME"\") {
    UDF_"$QNAME" udf(Request, ServiceApi);
    return this->Run(udf);
  }
  return true;
}//End RunQuery
}  // namepsace UDIMPL\n" >> /tmp/baseline.cpp

#compile command
COMPILE="/usr/bin/g++ -DGOOGLE_DENSEHASHMAP -D_DataConsistency \"-DBUILDVERSION=\\\"product\\\"\" -Isrc/customer -Isrc/customer/core_impl/gpe_impl -Igsdk/include -Igsdk/include/third_party -Igsdk/include/third_party/sparsehash/usr/local/include -Igsdk/include/third_party/glog/usr/local/include -Igsdk/include/third_party/jsoncpp/include -Igsdk/include/third_party/redis/deps/hiredis -Igsdk/include/core/gpe -Igsdk/include/core/gse -Igsdk/include/olgp -Igsdk/include/core/topology -Igsdk/include/utility -Igsdk/include/realtime -c -Wall -fmessage-length=0 -Wno-unused -fpermissive -fno-omit-frame-pointer -ldl -DRELEASE -O3 -DREMOVETESTASSERT -D__STDC_FORMAT_MACROS -xc++ - -o objs/querydispatcher.o"

# goto gdk folder, and compile udf
cd $PRODUCT/gdk/
eval $COMPILE < /tmp/baseline.cpp

# get result
RC=$?
if [ $RC -ne 0 ]
then
  echo "Compile error!"
  echo "Basle line "$file
  exit $RC
fi

done

echo "All correct!"
