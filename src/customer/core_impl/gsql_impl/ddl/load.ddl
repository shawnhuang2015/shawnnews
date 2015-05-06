drop job load_poc_graph
create loading job load_poc_graph for graph poc_graph
  load "src/ddl/data.csv"
    to edge e_thru values ($ip, $session_id, $time)
  using header="true", separator=",";
end
