# -*- mode: python; -*-
# build file for Graphsql CSE and GPE
# Requirement: 
# 1. scons (and python), from http://www.scons.org
# 2. boost library(included)
# 3. yaml-cpp (included)
# 4. CMake
# 5. bison >= 2.7
# 6. flex  >= 2.5.37
  
# This file, SConstruct, configures the build environment, and then delegates to
# several, subordinate SConscript files, which describe specific build rules.
# Here setup the common env, release env and debug_env first, then add
# separate projects to build.

import os, sys
import subprocess
import shutil
# add the path to the test_utils library
#sys.path.append('testing/site_scons')
#from test_utils import *
sys.path.append('.site_scons')
from build_utils import *
            

# Get scons itself
scons_executable = sys.argv[0]

############################################################
## 0.0 Define a common build environment                   #
############################################################
common_env = Environment(ENV = {'PATH' : os.environ['PATH']})
common_env['GSQLOPTIONS']=[]

############################################################
## 0.1. add your own command options                       #
############################################################
AddNewBoolOption(
        common_env,
        '--parser',
        'gsql_parser',
        True,
        'Build parser.',
        False)
AddNewCMDOption(
        common_env,
        '--gcppflags',
        'gcppflags',
        'string',
        '=',
        'graphsql cppflags',
        '')
AddNewCMDOption(
        common_env,
        '--glinkflags',
        'glinkflags',
        'string',
        '=',
        'graphsql linkflags',
        '')
AddNewCMDOption(
        common_env,
        '--CPPDEFINES',
        'gcppdefines',
        'string',
        '=',
        '"D1,D2=10,..."',
        '')

############################################################
##  HELP                                                   #
############################################################

if 'test' in COMMAND_LINE_TARGETS:
    PrintTestHelp()
else:
    PrintBuildHelp(common_env)

############################################################
## 0. Define a common build environment                    #
## and then clone a release and a debug environment        #
############################################################
# Build path
RELEASE_DIR = 'build/release'
DEBUG___DIR = 'build/debug'

#----- if verbose is true(verbose!=0), no color in output
SetupCommonBuildEnv(common_env, int(ARGUMENTS.get('verbose', 0))!=0)

# ----- Release build is derived from the common build environment...
release_env = common_env.Clone()
SetupReleaseBuildEnv(release_env, RELEASE_DIR)


# If the build is for customer, we need to remove the testing assert to improve performance.
if int(ARGUMENTS.get('cb',0)) > 0 :
    release_env.Append(CPPDEFINES=['REMOVETESTASSERT'])
    

# ----- Define debug build environment in a similar fashion...
debug___env = common_env.Clone()
SetupDebugBuildEnv(debug___env, DEBUG___DIR)


############################################################
## 1. setup the third party libraries first                #
##   - linenoise/murmurhash/snappy/jsoncpp                 #
##   - boost (w/ extra boostthreadpool)                    #
##   - yaml-cpp                                            #
##   - sparsehash                                          #
##   - glog                                                #
##   - gtest                                               #
##   - redis                                               #
############################################################
current_dir = os.getcwd()

# build linenoise, murmurhash and snappy. They have both debug and release build
debug___env.SConscript(DEBUG___DIR+'/third_party/SConscript', {'env': debug___env})
release_env.SConscript(RELEASE_DIR+'/third_party/SConscript', {'env': release_env})


# build boost and yaml-cpp with their own builders(bjam and CMake). 
# Note all libraries are release builds.

if not common_env.GetOption('clean') and not common_env.GetOption('help'):
    os.chdir('src/third_party')
    """ create a list of tuples: each has this format:
        $packagekey, $representative_target
    You need to provide:
        gsql_$packagekey.sh     --> the build shell script
        graphsq_$packagekey*.*  --> the zipped pkg src file
    In above,
        $packagekey is the package name and will be used as directory name
        $representative_target is an example library file built from source.
    The rule of triggering the build is one of below:
        1) directory src/third_party/$packagekey doesn't exist, or
        2) file $representative_target doesn't exist, or
        3) source file graphsql_$packagekey*.* has a different size than the previous build
    """
    third_party_list=[
    ('boost',       'boost/release/lib/libboost_thread.a'),
    ('glog',        'glog/usr/local/lib/libglog.a'),
    ('gtest',       'gtest/libgtest.a'),
    ('redis',       'redis/deps/hiredis/libhiredis.a'),
    ('sparsehash',  'sparsehash/usr/local/include/sparsehash/template_util.h'),
    ('networkx',    'networkx/PKG-INFO'),
    ('yaml',        'yaml/usr/local/lib/libyaml-cpp.a'),
    ('zlib',        'zlib/libz.a'),
    ('zookeeper_c', 'zookeeper_c/lib/libzookeeper_mt.a'),
    ('zeromq',      'zeromq/lib/libzmq.a'),
    ('play',        'play/play-2.2.0.zip')
    ]
    # -- make sure the elements in both lists match
    # -- the elements in the second list are the built objects. If a package has
    #    more than one targets (e.g., boost), then choose one.
    build_all_thirdparty(third_party_list)

    #come back from third_party dir
    os.chdir(current_dir)
    

###############################################################
# 2 : build parser is not default as github has the generated #
#      C++ files. To regenerate parser, use option "--parser" #
###############################################################
if  GetOption('gsql_parser') and \
        not common_env.GetOption('clean') and\
        not common_env.GetOption('help'):
    os.chdir('src/graphsql/server/parser')
    retcode=subprocess.call(scons_executable)
    if (retcode !=0 ):
        print " Building graphsql parser failed with code ", retcode
        sys.exit(-2)
    os.chdir(current_dir)
    

#############################################################
# 3 : build different projects in debug or release mode     #
#############################################################

# Now that all build environment have been defined, let's define each project
#------------------------------------
# 3.1.0 Project: gse2               |
#------------------------------------
gse2_debug___build = debug___env.SConscript(DEBUG___DIR +
                        '/core/gse/SConscript', {'env': debug___env})
gse2_release_build = release_env.SConscript(RELEASE_DIR +
                        '/core/gse/SConscript', {'env': release_env})


#---------------------------------------
# 3.1.2 Project: gpelib and other libs |
#---------------------------------------
gpelib_debug___build = debug___env.SConscript(DEBUG___DIR + 
                        '/core/gpe/SConscript', {'env': debug___env})
gpelib_release_build = release_env.SConscript(RELEASE_DIR + 
                        '/core/gpe/SConscript', {'env': release_env})


distlib_debug___build = debug___env.SConscript(DEBUG___DIR + 
                        '/msg/distributed/SConscript', {'env': debug___env})
distlib_release_build = release_env.SConscript(RELEASE_DIR + 
                        '/msg/distributed/SConscript', {'env': release_env})


kafka_perf_producer_daemon_debug___build = debug___env.SConscript(DEBUG___DIR +
                        '/msg/distributed/perf_test_utils/SConscript', {'env': debug___env})
kafka_perf_producer_daemon_release_build = release_env.SConscript(RELEASE_DIR +
                        '/msg/distributed/perf_test_utils/SConscript', {'env': release_env})

kafka_perf_consumer_daemon_debug___build = debug___env.SConscript(DEBUG___DIR +
                        '/msg/distributed/perf_test_utils/SConscript', {'env': debug___env})
kafka_perf_consumer_daemon_release_build = release_env.SConscript(RELEASE_DIR +
                        '/msg/distributed/perf_test_utils/SConscript', {'env': release_env})

#############################################################
# 3.1.6: POC GSE BASE                                       #
#############################################################
## poc_gse ##
poc_gseloader_debug___build = debug___env.SConscript(DEBUG___DIR +
                '/customer/core_ext/gse/SConscript', {'env': debug___env})
poc_gseloader_release_build = release_env.SConscript(RELEASE_DIR +
                '/customer/core_ext/gse/SConscript', {'env': release_env})

#############################################################
# 3.1.7: POC GPE BASE                                       #
#############################################################
## poc_gpe ##
poc_gpe_debug_build = debug___env.SConscript(DEBUG___DIR + 
                '/customer/core_ext/gpe/SConscript', {'env': debug___env})
poc_gpe_release_build = release_env.SConscript(RELEASE_DIR + 
                '/customer/core_ext/gpe/SConscript', {'env': release_env})
                
# Set up general udf debug (for local test only)
gpe_poc_udf_debug_env = debug___env.Clone()
poc_udf_cmdline_build = release_env.SConscript(RELEASE_DIR + '/customer/core_ext/gpe/SConscript_udf', {'env': release_env})
release_env.Alias('poc_udf_cmdline',poc_udf_cmdline_build)
gpe_udf_debug_build = gpe_poc_udf_debug_env.SConscript(DEBUG___DIR + '/customer/core_ext/gpe/SConscript_udf', {'env': gpe_poc_udf_debug_env})
gpe_poc_udf_debug_env.Alias('gpe_udf_debug', gpe_udf_debug_build)

#------------------------------------------------------
# 3.2 Determine default project to build, e.g. 'scons'|
#------------------------------------------------------
debug___env.Default(gse2_debug___build,
                    gpelib_debug___build,
                    distlib_debug___build)
#                    kafka_perf_producer_daemon_debug___build,
#                    kafka_perf_consumer_daemon_debug___build)
release_env.Default(gse2_release_build,
                    gpelib_release_build,
                    distlib_release_build)
#                    kafka_perf_producer_daemon_release_build,
#                    kafka_perf_consumer_daemon_release_build)

#---------------------------------------------------------------------
# 3.3 Determine a specific project to build, e.g. 'scons  gse2_debug' |
#---------------------------------------------------------------------
debug___env.Alias('gse2_debug',         gse2_debug___build)
debug___env.Alias('gpelib_debug',       gpelib_debug___build)
debug___env.Alias('distlib_debug',      distlib_debug___build)
debug___env.Alias('kafka_perf_producer_daemon_debug',      kafka_perf_producer_daemon_debug___build)
debug___env.Alias('kafka_perf_consumer_daemon_debug',      kafka_perf_consumer_daemon_debug___build)
debug___env.Alias('poc_gseloader_debug',poc_gseloader_debug___build)
debug___env.Alias('poc_gpe_debug', poc_gpe_debug_build)

release_env.Alias('gse2_release',       gse2_release_build)
release_env.Alias('gpelib_release',     gpelib_release_build)
release_env.Alias('distlib_release',    distlib_release_build)
release_env.Alias('kafka_perf_producer_daemon_release',   kafka_perf_producer_daemon_release_build)
release_env.Alias('kafka_perf_consumer_daemon_release',   kafka_perf_consumer_daemon_release_build)
release_env.Alias('poc_gseloader_release', poc_gseloader_release_build)
release_env.Alias('poc_gpe_release', poc_gpe_release_build)

debug___env.Alias('debug',    
                    [
                    gse2_debug___build,
                    gpelib_debug___build,
                    distlib_debug___build
                    #kafka_perf_producer_daemon_debug___build,
                    #kafka_perf_consumer_daemon_debug___build
                    ]
                  )
release_env.Alias('release',  
                    [
                    gse2_release_build,
                    gpelib_release_build,
                    distlib_release_build
                    #kafka_perf_producer_daemon_release_build,
                    #kafka_perf_consumer_daemon_release_build
                    ]
                 )


#############################################################
# 3.4 : PACKAGE -- all release builds go to pkg directory   #
#############################################################
#release_env.Append(TARFLAGS='-c -z')
#release_env.Append(TARSUFFIX='.tar.gz')
pkgdir = 'graphsql_'+release_env["BUILD_VERSION"]
# remove graphsql_bin/rebuild dir so we always install the latest
if 'pkg' in COMMAND_LINE_TARGETS:
    rebuildDir = pkgdir+'/rebuild'
    subprocess.call('rm -rf '+rebuildDir, shell=True)

release_env.Install(pkgdir, [
                            poc_gseloader_release_build,
                            gse2_release_build
                            ]
                    )
release_env.Alias('pkg', pkgdir)

#############################################################
# 4 : doxy document                                         #
#############################################################
doxybld = Builder(action = '/opt/local/bin/doxygen $SOURCE')
common_env['BUILDERS']['gen_doxydocs'] = doxybld
common_env.Alias('docs', common_env.gen_doxydocs('graphsql_doxy.cfg'))

#############################################################
# 5 : unit test build                                       #
#############################################################
# Don't pollute the common environments.
unit___debug_env = debug___env.Clone()
unit_release_env = release_env.Clone()
# Set up include and lib to the unit test
gtestlib = File('#/src/third_party/gtest/libgtest.a')
unit___debug_env.Append(CPPPATH=["#/src/third_party/gtest/include"]);
unit_release_env.Append(CPPPATH=["#/src/third_party/gtest/include"]);
unit___debug_env.Append(LIBS=[gtestlib])
unit_release_env.Append(LIBS=[gtestlib])

# Set up the debug and release targets
# unit___debug_build = unit___debug_env.SConscript(DEBUG___DIR + 
#                 '/unittests/SConscript', {'env': unit___debug_env})
# unit_release_build = unit_release_env.SConscript(RELEASE_DIR + 
#                 '/unittests/SConscript', {'env': unit_release_env})
# # Set up the keywords
# unit___debug_env.Alias('unit_debug',    unit___debug_build)
# unit_release_env.Alias('unit_release',  unit_release_build)
# unit_release_env.Alias('unit',  unit_release_build)

#############################################################
# 6 : Testing framework                                     #
#############################################################
if 'test' in COMMAND_LINE_TARGETS:
    # let's pass on all the test options. Simple, just get rid of
    # the first argument, and remove "-D" and "test":
    # e.g., commandline is:
    # "scons test ????"
    # "scons -D test ????"
    testOption=" "+" ".join(sys.argv[1:])+" "
    testOption = testOption.replace(" test ", " ");
    testOption = testOption.replace(" -D ", " ");
    testOption = testOption.strip()
    if len(testOption) > 1 :
        print "Running test with option \"" + testOption + "\""
    os.environ["GRAPHSQL_BIN_DIR"] = os.path.abspath('graphsql_bin')
    os.environ["GRAPHSQL_TEST_ROOT"] = os.path.abspath('testing')
    os.environ["PYTHONPATH"]=os.path.abspath('src/third_party/networkx/lib/python2.7/site-packages/')
    os.chdir('testing/tests')
    retcode = subprocess.call([scons_executable,testOption])
    if (retcode !=0 ):
        print " Tests failed with code ", retcode
        sys.exit(-6)
    else:
        sys.exit(0)


#############################################################
# 10: customer POC builds                                   #
#############################################################


#############################################################
# 11: D2D builds                                            #
#############################################################


#---------------------------------------
# 11.2 gse_mock_worker                 |
#---------------------------------------
# Set up the debug and release targets
gse_mock_worker_debug_build = debug___env.SConscript(DEBUG___DIR +
                '/core/gse/gse2/test/SConscript', {'env': debug___env})
gse_mock_worker_release_build = release_env.SConscript(RELEASE_DIR +
                '/core/gse/gse2/test/SConscript', {'env': release_env})
# Set up the keywords
debug___env.Alias('gse_mock_debug', gse_mock_worker_debug_build)
release_env.Alias('gse_mock_release', gse_mock_worker_release_build)
#---------------------------------------
#  distributed test target                   |
#---------------------------------------
dist_test_debug_build = unit___debug_env.SConscript(DEBUG___DIR +
                    '/msg/distributed/SConscriptTest', {'env': unit___debug_env})
dist_test_release_build = unit_release_env.SConscript(RELEASE_DIR +
                   '/msg/distributed/SConscriptTest', {'env': unit_release_env})
# Set up the keywords
unit___debug_env.Alias('dist_test_debug',    dist_test_debug_build)
unit_release_env.Alias('dist_test_release',  dist_test_release_build)

debug___env.Alias('dist_debug',
                    [
                    distlib_debug___build,
                    dist_test_debug_build
                    ]
                  )
release_env.Alias('dist_release',
                    [
                    distlib_release_build,
                    dist_test_release_build
                    ]
                 )

