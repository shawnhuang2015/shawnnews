#!/bin/bash
rm -rf src/customer/core_impl/gpe_impl/query
mkdir -p src/customer/core_impl/gpe_impl/query; cp -r src/gle/engine/acclib  src/customer/core_impl/gpe_impl/query 
./gpkg.sh
cd src
./publish_gsdk.sh
cd ..
rm -rf cpkg
mkdir -p cpkg/gdk/src/er
mkdir -p cpkg/gdk/gsdk
rsync -avL --progress gdk cpkg --exclude=gdk/src --exclude=gdk/gsdk
cp -RL deploy_gsdk/gsdk cpkg/gdk 
cp -RL src/customer cpkg/gdk/src
cp -RL src/er/buildenv cpkg/gdk/src/er
cp -RL src/er/platform_independent cpkg/gdk/src/er
cp -RL src/er/tools cpkg/gdk/src/er
cp -RL src/er/misc cpkg/gdk/src/er
cp -RL src/visualization cpkg/gdk/src
cp -RL gsql cpkg/gdk/ 2>/dev/null || :
