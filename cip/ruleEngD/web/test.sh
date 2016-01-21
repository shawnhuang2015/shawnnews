#!/bin/sh

request_body=$(cat <<-EOF
{
    "actor":
    {
        "flow_type":"Lakala Riks",
        "entry_point":"http://lakala.payment",
        "client_id":"001",
        "ip":"192.168.1.33",
        "session_id":"898SDFI89SEF"
    },
    "rule_suite":
    {
        "uri":"http://cip.graphsql.com/lakla/testrulesuite",
        "conflict_resolution":"HPW",
        "check_point":"testcheckpoint",
        "summary":"fraud check"
    },
    "context":
    {
        "user_id":"190012381",
        "name":"Will Smith",
        "gender":"Male",
        "age":"86",
        "current_position":"http://lakala/cart",
        "login_time":"20160101",
        "isVip":"false"
    }
}
EOF
)

url='http://192.168.33.70:5000/cip/api/1.0/ruleng'
curl -H 'Content-Type:application/json' -X POST "$url" -d "$request_body" | python -m json.tool
