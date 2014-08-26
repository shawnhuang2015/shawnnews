ABOUT THIS FOLDER:

conf_poc4_base.yaml -- the deployment config for this project.  Unless you are doing something
really sophisticated you probably won't have to change much in here, unless you change the name
of the graph in graph_config.yaml (the name needs to be the same in both files)

load_test_conf.yaml -- similar to above, but you can use this to test your data loader and
initialize a graph store without loading Zookeeper, kafka, etc.

graph_config.yaml -- the system uses this file to parse your graph schema.  it is initially
configured for an unattributed, undirected graph.  You should probably modify this.

test.property -- this file is passed to the loader executable. It is set up so that if you run
the gse_loader from the repo root, it will find all of your conf files.  You need to edit this
file by placing a list of the input files you need at the end of the file.
