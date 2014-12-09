C_PWD=${PWD}
echo $C_PWD
export IUM_HOME=~/dev/ium/mgnt_cli

sed -e 's%\/SOME_DIR%'"$C_PWD"'%' local_conf.yaml.template > local_conf.yaml
cp local_conf.yaml $IUM_HOME/configs/

cd ../../../..
#./package_all.sh -d
cp dist_test_pkg.tar.gz $IUM_HOME/pkgs/

cd $IUM_HOME
fab -f mgnt_cli.py install_pkg:dist_test_pkg,local_conf,True
