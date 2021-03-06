#!/bin/sh

url='http://192.168.33.70:9000/gpath'
request_body=$(cat <<-EOF
{
    "query":"imsi=>.project{imsi._external_id}  as A >> out",
    "limit":{
        "A":1000
    }
}
EOF
)
echo "Preparing data, please wait in patience ..."
result=`curl -s -H 'Content-Type:application/json' -X POST "$url" -d "$request_body" | python -m json.tool`
ids=`echo $result | jq '.results.A.data' | grep \" | sed 's/"//g'`
url_re='http://192.168.33.70:5000/cip/api/1.0/ruleng'

echo "Done, press any key for calling rule engine"
read

for id in $ids
do
    echo ">>>Fraud detecting on imsi :$id  ...."

request_body_re=$(cat <<-EOF
{
    "actor":
    {
        "flow":"Bangcle Client Fraud Monitoring",
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
        "imsi":"$id"
    }
}
EOF
)

    curl -s -H 'Content-Type:application/json' -X POST "$url_re" -d "$request_body_re" | python -m json.tool | jq '.result.data.r1_same_imsi_finger' 
done
