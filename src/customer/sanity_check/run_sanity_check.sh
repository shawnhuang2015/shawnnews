# either move your vertex and edge data to the referenced files, or change these paths.  
$MGNT_HOME/scripts/load_data , $MGNT_HOME/sanity_check/vert.csv $MGNT_HOME/sanity_check/edg.csv

sleep 3

outfile=$MGNT_HOME/sanity_check/sanity_out.txt
checkfile=$MGNT_HOME/sanity_check/sanity_check.txt

rm -rf $outfile

#add a valid ID to this command to get started!
#curl -X GET 'http://localhost:9000/debug_neighbors/<VALID_ID>' >> $outfile

#printf "\n" >>$outfile

# TODO: add more curl commands here for each of your tests.


if diff $outfile $checkfile >/dev/null ; then
    echo "Test Passed!"
else
    echo "Test Failed."
fi