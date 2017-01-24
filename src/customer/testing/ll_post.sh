set -x
set -e

#gsql_admin restart

curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/transaction' \
     -d '{"txn":"txn2","ip":"ip1","userid":"userid1","fraud":"false"}'
curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/transaction' \
     -d '{"txn":"txn3","ip":"ip1","bankid":"bankid3","ssn":"ssn3","fraud":"false"}'
curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/transaction' \
     -d '{"txn":"txn4","ip":"ip4","bankid":"bankid3","ssn":"ssn3","fraud":"false"}'
curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/transaction' \
     -d '{"txn":"txn5","ip":"ip4","userid":"userid5","ssn":"ssn5","fraud":"false"}'
curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/transaction' \
     -d '{"txn":"txn6","ssn":"ssn5","fraud":"false"}'
curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/transaction' \
     -d '{"txn":"txn3","userid":"userid5","fraud":"false"}'
curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/transaction' \
     -d '{"txn":"txn5","bankid":"bankid3","fraud":"false"}'

# set black
curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/transaction' \
     -d '{"txn":"txn3","fraud":"true"}'
curl -H 'Content-Type:application/json' -X POST 'http://localhost:9000/transaction' \
     -d '{"txn":"txn6","fraud":"true"}'

# score=2520
curl -X GET 'http://localhost:9000/transactionfraud?id=txn4'
# score=100
curl -X GET 'http://localhost:9000/transactionfraud?id=txn2'
# score=3010
curl -X GET 'http://localhost:9000/transactionfraud?id=txn5'
# score=500
curl -X GET 'http://localhost:9000/transactionfraud?id=txn6'
# score=1500
curl -X GET 'http://localhost:9000/transactionfraud?id=userid5&type=userid'
# score=20
curl -X GET 'http://localhost:9000/transactionfraud?id=ip4&type=ip'
