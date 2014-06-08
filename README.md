product
=======

Product and Projects

Quick start:
- edit config/proj.conf if necessary
- "./gworkspace.sh -u" to update the workspace
- source ". .gsql_setup_dev_env.source" to setup the dev env.
- coding...
   if you are change more than one repo, run "./gworkspace.sh -s" to
   check it any repo has change (dirty). Once commit, do remember
   to push in each repo. "./gworkspace.sh -i" will give more info.
- build package
   * edit config/pkg.config if needed. Don't sh this file even it's executable.
   * you may also edit src/customer/config files
   * ./gpkg.sh
