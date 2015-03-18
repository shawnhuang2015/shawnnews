CREATE VERTEX transaction(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)
CREATE VERTEX userId(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)
CREATE VERTEX ssn(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)
CREATE VERTEX bankId(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)
CREATE VERTEX cell(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)
CREATE VERTEX imei(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)
CREATE VERTEX ip(PRIMARY_ID value string NOT NULL, isFraudulent bool NULL)

CREATE UNDIRECTED EDGE transToEntity(FROM *, TO *)

CREATE GRAPH lianlian(transaction, userId, ssn, bankId, cell, imei, ip, transToEntity)



CREATE LOADING JOB lianlianLoad FOR GRAPH lianlian
  LOAD "/Users/Ben/Programming/jodi365/testdata.csv" TO VERTEX transaction VALUES($0, "false"),
          TO VERTEX userId VALUES($8,"false"),
          TO VERTEX ssn VALUES($15,"false") WHERE IsSSNIDType($16),
          TO VERTEX bankId VALUES($11,"false"),
          TO VERTEX cell VALUES($14,"false"),
          TO VERTEX imei VALUES($20,"false"),
          TO VERTEX ip VALUES($18,"false")
       USING HEADER="false", SEPARATOR=",";
  LOAD "/Users/Ben/Programming/jodi365/testdata.csv" TO EDGE transToEntity VALUES($0, $8)
       USING HEADER="false", SEPARATOR=",", FROM="transaction", TO="userId";
  LOAD "/Users/Ben/Programming/jodi365/testdata.csv" TO EDGE transToEntity VALUES($0, $15)
       USING HEADER="false", SEPARATOR=",", FROM="transaction", TO="ssn";
  LOAD "/Users/Ben/Programming/jodi365/testdata.csv" TO EDGE transToEntity VALUES($0, $11) WHERE IsSSNIDType($16)
       USING HEADER="false", SEPARATOR=",", FROM="transaction", TO="bankId";
  LOAD "/Users/Ben/Programming/jodi365/testdata.csv" TO EDGE transToEntity VALUES($0, $14)
       USING HEADER="false", SEPARATOR=",", FROM="transaction", TO="cell";
  LOAD "/Users/Ben/Programming/jodi365/testdata.csv" TO EDGE transToEntity VALUES($0, $20)
       USING HEADER="false", SEPARATOR=",", FROM="transaction", TO="imei";
  LOAD "/Users/Ben/Programming/jodi365/testdata.csv" TO EDGE transToEntity VALUES($0, $18)
       USING HEADER="false", SEPARATOR=",", FROM="transaction", TO="ip";
END
