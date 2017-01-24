#!/bin/sh

request_body=$(cat <<-EOF
{
    "actor":
    {
        "flow":"Bangcle device",
        "entry":"",
        "client_id":"002",
        "ip":"192.168.1.33",
        "session_id":"898SDFI89SEF"
    },
    "checkpoint":
    {
        "name":"OnDeviceInit",
        "uri":"http://cip.graphsql.com/bangcle/ondeviceinit",
        "conflict":"ALL",
        "summary":"fraud check"
    },
    "event":
    {
        "ts":190012381,
        "imsi":"787OIU78786ll"
    }
}
EOF
)

url='http://192.168.33.70:5000/cip/api/1.0/ruleng'
curl -H 'Content-Type:application/json' -X POST "$url" -d "$request_body" | python -m json.tool
