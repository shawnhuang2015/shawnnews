# either move your vertex and edge data to the referenced files, or change these paths.  
$MGNT_HOME/scripts/load_data , $MGNT_HOME/sanity_check/vert.csv $MGNT_HOME/sanity_check/edg.csv

sleep 3

rm -rf sanity_out.txt

#add a valid ID to this command to get started!
#curl -X GET 'http://localhost:9000/debug_neighbors/<VALID_ID>' >> sanity_out.txt

#printf "\n" >>sanity_out.txt

# TODO: add more curl commands here for each of your tests.  

if diff sanity_out.txt sanity_check.txt >/dev/null ; then
    echo "Test Passed!"
else
    echo "Test Failed."
fi