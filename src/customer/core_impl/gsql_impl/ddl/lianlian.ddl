CREATE VERTEX transaction(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)
CREATE VERTEX userId(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)
CREATE VERTEX ssn(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)
CREATE VERTEX bankId(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)
CREATE VERTEX cell(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)
CREATE VERTEX imei(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)
CREATE VERTEX ip(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)
CREATE UNDIRECTED EDGE transToEntity(FROM *, TO *)
CREATE GRAPH poc_graph(transaction, userId, ssn, bankId, cell, imei, ip, transToEntity)



CREATE LOADING JOB load_poc_graph FOR GRAPH poc_graph
  LOAD "/home/shawn.huang/data/lianlian/graphdata-sample-fraudadded.csv" TO VERTEX transaction VALUES($0,reduce(OR(IsNotNull($5)))),
          TO VERTEX userId VALUES($8,reduce(OR(IsNotNull($5)))) WHERE NotNull($8),
          TO VERTEX ssn VALUES($14,reduce(OR(IsNotNull($5)))) WHERE IsSSNIDTypeAndNotNull($15,$14),
          TO VERTEX bankId VALUES($10,reduce(OR(IsNotNull($5)))) WHERE NotNull($10),
          TO VERTEX cell VALUES($13,reduce(OR(IsNotNull($5)))) WHERE NotNull($13),
          TO VERTEX imei VALUES($19,reduce(OR(IsNotNull($5)))) WHERE NotNull($19),
          TO VERTEX ip VALUES($17,reduce(OR(IsNotNull($5)))) WHERE NotNull($17)
       USING HEADER="true", SEPARATOR=",";
  LOAD "/home/shawn.huang/data/lianlian/graphdata-sample-fraudadded.csv" TO EDGE transToEntity VALUES($0, $8) WHERE NotNull($8)
       USING HEADER="true", SEPARATOR=",", FROM="transaction", TO="userId";
  LOAD "/home/shawn.huang/data/lianlian/graphdata-sample-fraudadded.csv" TO EDGE transToEntity VALUES($0, $14) WHERE IsSSNIDTypeAndNotNull($15,$14)
       USING HEADER="true", SEPARATOR=",", FROM="transaction", TO="ssn";
  LOAD "/home/shawn.huang/data/lianlian/graphdata-sample-fraudadded.csv" TO EDGE transToEntity VALUES($0, $10) WHERE NotNull($10)
       USING HEADER="true", SEPARATOR=",", FROM="transaction", TO="bankId";
  LOAD "/home/shawn.huang/data/lianlian/graphdata-sample-fraudadded.csv" TO EDGE transToEntity VALUES($0, $13) WHERE NotNull($13)
       USING HEADER="true", SEPARATOR=",", FROM="transaction", TO="cell";
  LOAD "/home/shawn.huang/data/lianlian/graphdata-sample-fraudadded.csv" TO EDGE transToEntity VALUES($0, $19) WHERE NotNull($19)
       USING HEADER="true", SEPARATOR=",", FROM="transaction", TO="imei";
  LOAD "/home/shawn.huang/data/lianlian/graphdata-sample-fraudadded.csv" TO EDGE transToEntity VALUES($0, $17) WHERE NotNull($17)
       USING HEADER="true", SEPARATOR=",", FROM="transaction", TO="ip";
END
