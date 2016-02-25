while true
do
    now=$(date +%s000000000)
    curl -i -XPOST 'http://localhost:8086/write?db=ruleng' --data-binary "sla,checkpoint=OnDeviceInit value=$RANDOM $now"
    now=$(date +%s000000000)
    curl -i -XPOST 'http://localhost:8086/write?db=ruleng' --data-binary "sla,checkpoint=OnUserLogin value=$RANDOM $now"
    sleep 1
done
