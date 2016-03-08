#!/bin/bash

#import ontology
curl -X POST 'http://localhost:9000/ontology_import?object=user&name=tag' --data-binary @tag.csv
curl -X POST 'http://localhost:9000/ontology_import?object=pic&name=pic_tag' --data-binary @pic_tag.csv
curl -X POST 'http://localhost:9000/ontology_import?object=user&name=interest' --data-binary @interest.csv

echo "sleep 10 seconds ..."
sleep 10

#set user-tag
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=tag&sep=;' -d 'user0;gender.male|1.0'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=tag&sep=;' -d 'user2;gender.male|1.0'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=tag&sep=;' -d 'user4;gender.male|1.0'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=tag&sep=;' -d 'user1;gender.female|1.0'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=tag&sep=;' -d 'user3;gender.female|1.0'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=tag&sep=;' -d 'user5;gender.female|1.0'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=tag&sep=;' -d 'user0;age.10-20|1.0'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=tag&sep=;' -d 'user1;age.10-20|1.0'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=tag&sep=;' -d 'user2;age.10-20|1.0'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=tag&sep=;' -d 'user3;age.20-30|1.0'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=tag&sep=;' -d 'user4;age.20-30|1.0'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=tag&sep=;' -d 'user5;age.20-30|1.0'

#set user-interest
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=interest&sep=;' -d 'user0;food|1.0'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=interest&sep=;' -d 'user1;food|0.7'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=interest&sep=;' -d 'user2;food|0.5'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=interest&sep=;' -d 'user3;shoes|0.8'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=interest&sep=;' -d 'user4;shoes|0.6'
curl -X POST 'http://localhost:9000/set_user_tag?object=user&name=interest&sep=;' -d 'user5;shoes|0.4'

#set pic-pic_tag
curl -X POST 'http://localhost:9000/set_item_tag?object=pic&name=pic_tag&sep=;' -d 'pic0;blue'
curl -X POST 'http://localhost:9000/set_item_tag?object=pic&name=pic_tag&sep=;' -d 'pic1;blue'
curl -X POST 'http://localhost:9000/set_item_tag?object=pic&name=pic_tag&sep=;' -d 'pic2;red'
curl -X POST 'http://localhost:9000/set_item_tag?object=pic&name=pic_tag&sep=;' -d 'pic3;red'


#curl -X GET 'http://localhost:9000/get_tag?object=user&id=u1&name=tag&verbose=1'
#curl -X GET 'http://localhost:9000/get_profile'
#curl -X GET 'http://localhost:9000/get_ontology?name=tag'
#curl -X POST 'http://localhost:9000/clear_semantic'
