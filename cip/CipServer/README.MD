# CIP Server
This is the initial framework for CIP Server.
The readme will help to setup the developing environment for OS-X as well as provide some useful information for development.

##Setup Guide

### Installation
  #machine: MAC OS
    $ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    $brew install node
    $brew install mongodb; mkdir -p /data/db;mongod &
    $npm install -g gulp
    $npm install -g bower
    $npm install -g mean-cli

    $cd product/cip/CipServer
    $npm install
    $bower install

  #machine: Linux
    $git clone https://github.com/creationix/nvm.git ~/.nvm
    $source ~/.nvm/nvm.sh
    $nvm install node_version

    $download mongdb: wget https://fastdl.mongodb.org/linux/mongodb-linux-x86_64-rhel62-3.2.3.tgz
    $mkdir -p /data/db
    $cd /mongodb/bin; sudo ./mongod &

    $npm install -g gulp
    $npm install -g bower
    $npm install -g mean-cli

    $cd product/cip/CipServer
    $npm install
    $bower install

### Start run
    $gulp 

## Developing Guide

### Package Catalog


