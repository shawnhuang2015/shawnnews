drop all

create vertex v_ip (primary_id ip string, log string)
create vertex v_session (primary_id sid string)
create undirected edge e_thru (from v_ip, to v_session, time uint)

create graph poc_graph (v_ip, v_session, e_thru)
export schema poc_graph

CLEAR GRAPH STORE -HARD

set sys.data_root = "/home/shawn.huang/data/yeepay"

create loading job load_poc_graph for graph poc_graph
  load "$sys.data_root/data.csv"
    to vertex v_ip values ($ip, reduce(Concat($original_log))),
    to edge e_thru values ($ip, $session_id, $time)
  using header="true", separator=",", quote = "single";
#using header="true", separator=",", using quotes = "single|double";
end
