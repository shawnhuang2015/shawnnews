#!/bin/sh

request_body=$(cat <<-EOF
{
    "actor":
    {
        "flow":"Lakala Riks",
        "entry":"http://lakala.transaction",
        "client_id":"001",
        "ip":"192.168.1.33",
        "session_id":"898SDFI89SEF"
    },
    "checkpoint":
    {
        "name":"OnLakalaTran",
        "uri":"http://cip.graphsql.com/lakla/onTrans",
        "conflict":"HPW",
        "summary":"fraud check"
    },
    "event":
    {
        "merc_id":"190012381",
        "card_no":"Will Smith"
    }
}
EOF
)

url='http://192.168.33.70:5000/cip/api/1.0/ruleng'
curl -H 'Content-Type:application/json' -X POST "$url" -d "$request_body" | python -m json.tool
