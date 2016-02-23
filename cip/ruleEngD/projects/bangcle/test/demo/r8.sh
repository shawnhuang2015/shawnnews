#!/bin/sh

request_body=$(cat <<-EOF
{
    "actor":
    {
        "flow":"Bangcle login",
        "entry":"",
        "client_id":"002",
        "ip":"192.168.1.33",
        "session_id":"898SDFI89SEF"
    },
    "checkpoint":
    {
        "name":"OnUserLogin",
        "uri":"http://cip.graphsql.com/bangcle/login",
        "conflict":"ALL",
        "summary":"fraud check"
    },
    "event":
    {
        "ts":190012381,
        "account":"123333111ADS"
    }
}
EOF
)

url='http://192.168.33.70:5000/cip/api/1.0/ruleng'
curl -H 'Content-Type:application/json' -X POST "$url" -d "$request_body" | python -m json.tool
