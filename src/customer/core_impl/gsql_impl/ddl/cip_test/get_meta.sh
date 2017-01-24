#!/bin/bash
get_meta() {
  obj=${1:-vertex}
  if [[ "$obj" == "vertex" ]]; then
    curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/builtins' -d  '{"function":"stat_vertex_number","type":"*"}'
  elif [[ "$obj" == "edge" ]]; then
    curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/builtins' -d  '{"function":"stat_edge_number","type":"*"}'
  fi
}