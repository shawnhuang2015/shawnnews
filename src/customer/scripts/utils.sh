get_id() {
  rez=$1
  echo $rez | jq '.results[].id' | tr -d '\"'
}

get_hotness() {
  zid=$1
  pid=$2
  [[ -z $zid ]] || [[ -z $pid ]] && { >&2 echo "empty"; return 1; }
  curl -X GET "http://localhost:9000/graph/edges/product/$pid/zone_product/zone/$zid" | \
    jq '.results[].attributes.popularity'
}

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

del_edge() {
  svtype=$1
  svid=$2
  etype=$3
  if [[ $# -eq 3 ]]; then
    curl -X DELETE "http://localhost:9000/graph/edges/$svtype/$svid/$etype"
  else
    tvtype=$4
    tvid=$5
    curl -X DELETE "http://localhost:9000/graph/edges/$svtype/$svid/$etype/$tvtype/$tvid"
  fi
}

get_neighbor() {
  src_type=$1
  src_id=$2
  edge_type=$3
  limit=${4:-1}
  curl -X POST 'http://localhost:9000/builder' -d "{\"alg_control\":[{\"start_nodes\":[{\"type\":\"$src_type\", \"id\":\"$src_id\"}], \"edge_types\":[{\"type\":\"$edge_type\"}], \"filters\":{\"limit\":\"$limit\"}}]}"
}

get_vattr() {
  src_type=$1
  src_id=$2
  attrs=$3
  curl -X GET "http://localhost:9000/graph/vertices/$src_type/$src_id?select=$attrs"
}

get_eattr() {
  src_type=$1
  src_id=$2
  etype=$3
  attrs=$4
  if [[ $# -lt 5 ]]; then
    curl -X GET "http://localhost:9000/graph/edges/$src_type/$src_id/$etype?select=$attrs"
  else
    tgt_type=$5
    tgt_id=$6
    curl -X GET "http://localhost:9000/graph/edges/$src_type/$src_id/$etype/$tgt_type/$tgt_id?select=$attrs"
  fi
}

get_zone() {
  userid=$1
  cookies=($(curl -X GET "http://localhost:9000/graph/edges/userid/$userid/userid_cookie?select=date_time&sort=-date_time" | python -m json.tool | grep to_id | awk 'BEGIN{FS=":|,| +"} {gsub(/"/, "", $4); print $4}'))
  zone=1
  for c in "${cookies[@]}"; do
    echo $c - zone
    zones=($(curl -X GET "http://localhost:9000/graph/edges/cookieid/$c/cookie_zone?select=date_time&sort=-date_time" | python -m json.tool | grep to_id | grep to_id | awk 'BEGIN{FS=":|,| +"} {gsub(/"/, "", $4); print $4}'))
    [[ ${#zones[@]} -gt 0 ]] && zone=${zones[0]}
  done
  echo $zone
}

cuilder() {
  curl -X POST 'http://localhost:9000/builder' -d "$1"
}

get_vid() {
  tid=$1
  uid=$2
  curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/builtins' -d "{\"function\":\"vattr\", \"translate_typed_ids\": [{\"id\":\"$uid\",\"type\":$tid}]}" | jq '.results[].iid' | tr -d  '"'
}

add_cart_no_order() {
  curl -X POST 'http://localhost:9000/builder' -d ' {"alg_control": [{"start_nodes": [{"type": "product", "all_id": true}], "edge_types": [{"type": "add_cart"}], "filters": {"limit": 10}}]} '
}

get_meta() {
  obj=${1:-vertex}
  if [[ "$obj" == "vertex" ]]; then
    curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/builtins' -d  '{"function":"stat_vertex_number","type":"*"}'
  elif [[ "$obj" == "edge" ]]; then
    curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/builtins' -d  '{"function":"stat_edge_number","type":"*"}'
  fi
}
