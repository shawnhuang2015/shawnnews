##################################
## This is a script that copy the logs of certain gtest regress to base lines.
## You can choose which regress to copy, but please MAKE SURE the logs are totally correct.
## DON'T update base line w/o verification.
##################################

for it in 1 2 3 5 9 16 17 18 19 24 27
do
  #cp ./output/gsql/regress1/test${it}.log ./base_line/gsql/regress1/test${it}.base
  #cp ./output/gsql/regress2/test${it}.log ./base_line/gsql/regress2/test${it}.base
  #cp ./output/gsql/regress3/test${it}.log ./base_line/gsql/regress3/test${it}.base
  cp ./output/loader/regress1/test${it}.log ./base_line/loader/regress1/test${it}.base
done
