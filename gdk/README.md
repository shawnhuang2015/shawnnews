# gdk
GraphSQL SDK

0)[optional]
change config/proj.conf to specify where to get gsdk

1) Populate the workspace
./gworkspace.sh -u
(or ./gworkspace.sh -x https username password)

2) Build the whole package
./gpkg.sh

That is all. The pacakge is under the root directory of the product.


FYI: Build GPE Example:
>> make -f MakeDemo
>> bin/release/demo_gperun /data/rc4/gstore/0/part output_dir
