#!/bin/bash

g++ -g -I../../../../../customer -I../../../../../ -I../../../../../third_party -I../../../../../third_party/boost -I../../../../../third_party/sparsehash/usr/local/include -I../../../../../third_party/glog/usr/local/include -I../../../../../third_party/jsoncpp/include -I../../../../../core/gpe -I../../../../../core/gse -I../../../../../olgp -I../../../../../core/topology -I../../../../../utility test_context_table.cpp ../expression.cpp ../context_table.cpp ../dag_traversal.cpp ../aggregator.cpp -o test_context_table -L../../../../../../gdk/gsdk/lib/release -lolgplib -lgpelib -lboost_filesystem -lboost_system -lboost_thread -lpthread -ljson -ldl

#-L../../third_party/boost/release/lib -lboost_system
#-L/home/renchu.song/product/gdk/gsdk/lib/release -lolgplib -lgpelib -ldistlib -lnetlib -ljson -lsnappy -lyaml-cpp -lboost_filesystem -lboost_system -lboost_thread -lglog -lhiredis -lzookeeper_mt -lrdkafka -lz -lpthread -ldl
