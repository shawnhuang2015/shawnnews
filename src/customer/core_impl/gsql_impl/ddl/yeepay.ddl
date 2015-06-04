drop all

create vertex v_ip (primary_id ip string)
create vertex v_session (primary_id sid string)
create undirected edge e_thru (from v_ip, to v_session, time uint, log string)

create graph poc_graph (v_ip, v_session, e_thru)
export schema poc_graph

CLEAR GRAPH STORE -HARD

set sys.data_root = "/home/shawn.huang/data/yeepay"

create loading job load_poc_graph for graph poc_graph
  load "$sys.data_root/data.csv"
    to edge e_thru values ($ip, $session_id, $time, $original_log)
  using header="true", separator=",";
end
