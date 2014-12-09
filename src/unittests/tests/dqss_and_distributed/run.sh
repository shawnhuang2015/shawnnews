export GSQL_RECOMM_SYS_CONF=CUSTOM
export GSQL_RECOMM_SYS_CONF_PATH=$PWD/test_conf.yaml
TEST_DIR=${PWD}

# go to  IUM
: ${IUM_HOME:?"Need to set IUM_HOME before running this"}
cd $IUM_HOME
fab -f mgnt_cli.py install_pkg:dist_test_pkg,local_conf,True
# launch and init zookeepers
fab -f mgnt_cli.py launch_zookeepers
fab -f mgnt_cli.py init_zookeepers
# launch and init kafkas
fab -f mgnt_cli.py launch_kafkas
fab -f mgnt_cli.py init_kafkas
sleep 1

cd $TEST_DIR
#./kafka_setup.sh
# start test
../../../../build/release/unittests/gunit --gtest_filter=dqss.*
../../../../build/release/unittests/gunit --gtest_filter=dist.*

# DEBUG Version:
#GLOG_v=30 GLOG_logtostderr=1 ../../../../build/debug/unittests/gunit --gtest_filter=dqss.*

# need to shutdown zk and kafka
cd  $IUM_HOME
fab -f mgnt_cli.py shutdown_kafka || true
fab -f mgnt_cli.py shutdown_zookeeper

cd $TEST_DIR
rm -rf tmp/*
