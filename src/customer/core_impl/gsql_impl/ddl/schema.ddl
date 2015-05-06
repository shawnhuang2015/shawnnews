drop graph poc_graph cascade

create vertex v_ip (primary_id ip string)
create vertex v_session (primary_id sid string)
create undirected edge e_thru (from v_ip, to v_session, time uint)

create graph poc_graph (v_ip, v_session, e_thru)
export schema poc_graph
