#!/bin/bash

upsert_vertex() {
  vtype=$1
  vid=$2
  attr_str=$3  # attr1:val1,attr2:val2,...

  kvs=()
  attrs=($(echo ${attr_str//,/ }))
  for i in ${attrs[@]}; do
    i=($(echo ${i/:/ }))
    kvs=(${kvs[@]} "\"${i[0]}\":{\"value\":${i[1]}}")
  done
  kvs=$(IFS=','; echo "${kvs[*]}")

  echo "{\"vertices\":{\"$vtype\":{\"$vid\":{$kvs}}}}"
  curl -X POST 'http://localhost:9000/graph' -d "{\"vertices\":{\"$vtype\":{\"$vid\":{$kvs}}}}"
}

upsert_edge() {
  svtype=$1
  svid=$2
  etype=$3
  tvtype=$4
  tvid=$5
  attr_str=$6  # attr1:val1,attr2:val2,...

  kvs=()
  attrs=($(echo ${attr_str//,/ }))
  for i in ${attrs[@]}; do
    i=($(echo ${i/:/ }))
    kvs=(${kvs[@]} "\"${i[0]}\":{\"value\":${i[1]}}")
  done
  kvs=$(IFS=','; echo "${kvs[*]}")

  curl -X POST 'http://localhost:9000/graph'  -d " {\"edges\":{\"$svtype\":{\"$svid\":{\"$etype\":{\"$tvtype\":{\"$tvid\":{$kvs}}}}}}} "
}

upsert_vertex "user" "user0"
upsert_vertex "user" "user1"
upsert_vertex "user" "user2"
upsert_vertex "user" "user3"
upsert_vertex "user" "user4"
upsert_vertex "user" "user5"

upsert_vertex "browse" "browse0" "date_time:1450726395" 
upsert_vertex "browse" "browse1" "date_time:1451726395" 
upsert_vertex "browse" "browse2" "date_time:1452726395" 
upsert_vertex "browse" "browse3" "date_time:1453726395" 
upsert_vertex "browse" "browse4" "date_time:1454726395" 
upsert_vertex "browse" "browse5" "date_time:1455726395" 
upsert_vertex "browse" "browse6" "date_time:1456726395" 
upsert_vertex "browse" "browse7" "date_time:1457726395" 

upsert_vertex "pic" "pic0"
upsert_vertex "pic" "pic1"
upsert_vertex "pic" "pic2"
upsert_vertex "pic" "pic3"

upsert_edge "user" "user0" "user_browse" "browse" "browse0"
upsert_edge "user" "user0" "user_browse" "browse" "browse1"
upsert_edge "user" "user0" "user_browse" "browse" "browse2"
upsert_edge "user" "user1" "user_browse" "browse" "browse3"
upsert_edge "user" "user2" "user_browse" "browse" "browse4"
upsert_edge "user" "user2" "user_browse" "browse" "browse5"
upsert_edge "user" "user2" "user_browse" "browse" "browse6"
upsert_edge "user" "user2" "user_browse" "browse" "browse7"

upsert_edge "browse" "browse0" "browse_pic" "pic" "pic0"
upsert_edge "browse" "browse1" "browse_pic" "pic" "pic1"
upsert_edge "browse" "browse2" "browse_pic" "pic" "pic0"
upsert_edge "browse" "browse3" "browse_pic" "pic" "pic1"
upsert_edge "browse" "browse4" "browse_pic" "pic" "pic0"
upsert_edge "browse" "browse5" "browse_pic" "pic" "pic1"
upsert_edge "browse" "browse6" "browse_pic" "pic" "pic0"
upsert_edge "browse" "browse7" "browse_pic" "pic" "pic1"
