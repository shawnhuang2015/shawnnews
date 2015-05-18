CREATE VERTEX transaction(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL, label string)
CREATE VERTEX userId(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL, label string)
CREATE VERTEX ssn(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL, label string)
CREATE VERTEX bankId(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL, label string)
CREATE VERTEX cell(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL, label string)
CREATE VERTEX imei(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL, label string)
CREATE VERTEX ip(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL, label string)
CREATE UNDIRECTED EDGE transToEntity(FROM *, TO *)
CREATE GRAPH lianlian(transaction, userId, ssn, bankId, cell, imei, ip, transToEntity)



CREATE LOADING JOB lianlianLoad FOR GRAPH lianlian
  LOAD "/home/alan.lee/download/ll0.csv" TO VERTEX transaction VALUES($0,reduce(OR(IsNotNull($5))), $0),
          TO VERTEX userId VALUES($8,reduce(OR(IsNotNull($5))), $8) WHERE NotNull($8),
          TO VERTEX ssn VALUES($14,reduce(OR(IsNotNull($5))), $14) WHERE IsSSNIDTypeAndNotNull($15,$14),
          TO VERTEX bankId VALUES($10,reduce(OR(IsNotNull($5))), $10) WHERE NotNull($10),
          TO VERTEX cell VALUES($13,reduce(OR(IsNotNull($5))), $13) WHERE NotNull($13),
          TO VERTEX imei VALUES($19,reduce(OR(IsNotNull($5))), $19) WHERE NotNull($19),
          TO VERTEX ip VALUES($17,reduce(OR(IsNotNull($5))), $17) WHERE NotNull($17)
       USING HEADER="true", SEPARATOR=",";
  LOAD "/home/alan.lee/download/ll0.csv" TO EDGE transToEntity VALUES($0, $8) WHERE NotNull($8)
       USING HEADER="true", SEPARATOR=",", FROM="transaction", TO="userId";
  LOAD "/home/alan.lee/download/ll0.csv" TO EDGE transToEntity VALUES($0, $14) WHERE IsSSNIDTypeAndNotNull($15,$14)
       USING HEADER="true", SEPARATOR=",", FROM="transaction", TO="ssn";
  LOAD "/home/alan.lee/download/ll0.csv" TO EDGE transToEntity VALUES($0, $10) WHERE NotNull($10)
       USING HEADER="true", SEPARATOR=",", FROM="transaction", TO="bankId";
  LOAD "/home/alan.lee/download/ll0.csv" TO EDGE transToEntity VALUES($0, $13) WHERE NotNull($13)
       USING HEADER="true", SEPARATOR=",", FROM="transaction", TO="cell";
  LOAD "/home/alan.lee/download/ll0.csv" TO EDGE transToEntity VALUES($0, $19) WHERE NotNull($19)
       USING HEADER="true", SEPARATOR=",", FROM="transaction", TO="imei";
  LOAD "/home/alan.lee/download/ll0.csv" TO EDGE transToEntity VALUES($0, $17) WHERE NotNull($17)
       USING HEADER="true", SEPARATOR=",", FROM="transaction", TO="ip";
END
