drop all

create vertex BUS(primary_id BusId string, BusName string, NominalV float, PuV real, VAngle real, LoadP float, LoadQ float, GenP float, GenQ float, Switched float, GShunt float, BShunt float, Vr real, Vi real, Latitude float, Longitude float) with stats="OUTDEGREE_BY_EDGETYPE"

create directed edge BRANCH(from BUS, to BUS, CID string, R real, X real, hB real, Ratio real, Shift real, Type real, MVALimitA real, MVALimitB real, MVALimitC real)

create graph PowerFlow(BUS, BRANCH)


set sys.data_root="/home/shawnhuang/product/tmp/powergrid"

#create vertex BUS(primary_id BusId string, BusName string, NominalV float, PuV real, VAngle real, LoadP float, LoadQ float, GenP float, GenQ float, Switched float, GShunt float, BShunt float, Vr real, Vi real, Latitude float, Longitude float) with stats="OUTDEGREE_BY_EDGETYPE"

#create directed edge BRANCH(from BUS, to BUS, CID string, R real, X real, hB real, Ratio real, Shift real, Type real, MVALimitA real, MVALimitB real, MVALimitC real)


create loading job load_PowerFlow_data for graph PowerFlow
{  
  load "$sys.data_root/Bus_info.csv"
    to vertex BUS values($2, $3, $5, $6, $8, $9, $10, $11, $12, $13, $14, $15, 0, 0, $18, $19)
  using Separator=",", Header="true";

  load "$sys.data_root/Branch_info.csv"
    to edge BRANCH values($0, $2, $4, $8, $9, $10, $14, $15, $17, $11, $12, $13)
  using from="BUS", to="BUS", Separator=",", Header="true";
}

clear graph store -HARD

run job load_PowerFlow_data
