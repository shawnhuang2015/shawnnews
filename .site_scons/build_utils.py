# -*- mode: python; -*-
import os
import sys
import subprocess
import shutil
import glob

from SCons.Script.Main import AddOption
from SCons.Script.Main import GetOption
from SCons.Script.Main import SetOption
from SCons.Script.SConscript import *
from SCons.Script import COMMAND_LINE_TARGETS
from SCons.Script import ARGUMENTS
from SCons.Script import Split
from SCons.Script import File

def which(program):
    import os
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None


def localCopy(target, source, env):
    '''
        copy executable file
    '''
    shutil.copyfile(os.path.abspath(str(source[0])), os.path.abspath(str(target[0])))
    os.chmod(os.path.abspath(str(target[0])), 0755)

# color Env
def ColorEnv(env):
    '''
        Colorful env for CPP compilation
    '''
    colors = {}
    colors['cyan']   = '\033[96m'
    colors['purple'] = '\033[95m'
    colors['blue']   = '\033[94m'
    colors['green']  = '\033[92m'
    colors['yellow'] = '\033[93m'
    colors['red']    = '\033[91m'
    colors['end']    = '\033[0m'
    colors['bold']   = '\033[1m'

    #If the output is not a terminal, remove the colors
    if not sys.stdout.isatty():
       for key, value in colors.iteritems():
          colors[key] = ''

    compile_source_message = '%sCompiling file %s : %s$SOURCE%s' % \
       (colors['blue'], colors['yellow'], colors['purple'], colors['end'])

    compile_shared_source_message = '%sCompiling shared %s: %s$SOURCE%s' % \
       (colors['blue'], colors['yellow'], colors['purple'], colors['end'])

    install_message = '%sInstalling file %s: %s$SOURCE%s as %s%s$TARGET%s' % \
       (colors['red'], colors['yellow'],  colors['green'],  colors['end'], colors['green'], colors['bold'], colors['end'])

    link_program_message = '%sLinking Program %s: %s$TARGET%s' % \
       (colors['red'], colors['yellow'], colors['green'], colors['end'])

    link_library_message = '%sStatic Library %s : %s$TARGET%s' % \
       (colors['red'], colors['yellow'], colors['green'], colors['end'])

    ranlib_library_message = '%sRanlib Library  %s: %s$TARGET%s' % \
       (colors['red'], colors['yellow'], colors['green'], colors['end'])

    link_shared_library_message = '%sShared Library %s : %s$TARGET%s' % \
       (colors['red'], colors['yellow'], colors['green'], colors['end'])


    env['CXXCOMSTR'] = compile_source_message
    env['CCCOMSTR'] = compile_source_message,
    env['SHCCCOMSTR'] = compile_shared_source_message,
    env['SHCXXCOMSTR'] = compile_shared_source_message,
    env['ARCOMSTR'] = link_library_message,
    env['RANLIBCOMSTR'] = ranlib_library_message,
    env['SHLINKCOMSTR'] = link_shared_library_message,
    env['LINKCOMSTR'] = link_program_message,
    env['INSTALLSTR'] = install_message
    return env

# check if a file has different size than the last one
# if no change, return 0. If change, return the current size
def fileSizeChange(pre_size_file, cur_file_name):
    '''
        check if file size changed
    '''
    # Check the current file size
    curFile = filter(os.path.isfile, glob.glob(cur_file_name))
    # we may have more than one matched file. Add the size up.
    ActutalFileSize = 0
    for eachfile in curFile:
        ActutalFileSize = ActutalFileSize + os.stat(eachfile).st_size
    # check previous file size that is stored in file $pre_size_file
    if os.path.exists(pre_size_file):
        PreviousFileSize = 0
        with open(pre_size_file, 'r') as f:
            PreviousFileSize = int(f.readline())
        if (ActutalFileSize == PreviousFileSize):
            return 0
        else:
            return ActutalFileSize
    else:
        return ActutalFileSize


# excute shell command to build a third party lib
def build_thirdparty(onePackage, third_party_env):
    '''
        build a third party lib
    '''
    print "check " + onePackage[0] + " and "  + onePackage[1]
    needBuild = False
    #check if target exists
    if not os.path.exists(onePackage[0]) or not os.path.exists(onePackage[1]):
        needBuild = True
    pkgSizeFile = onePackage[0] + '/gsql_pkgsize'
    shellSizeFile = onePackage[0] + '/gsql_shellsize'
    newpkgSize = 0
    newshellSize = 0
    # check if pkg file size changes
    newpkgSize = fileSizeChange(pkgSizeFile,
                                "graphsql_" + onePackage[0]+"*")
    # check if shell file changes.
    newshellSize = fileSizeChange(shellSizeFile,
                                "gsql_" + onePackage[0]+".sh")
    if (needBuild + newshellSize > 0):
        needBuild = True
    if (needBuild):
        print "compiling "+onePackage[0]+" ..."
        retcode = subprocess.call(['sh', './gsql_'+onePackage[0]+'.sh'], env=third_party_env)
        if retcode != 0:
            print onePackage[0] +" installation failed with code ", retcode
            sys.exit(-1)
        else:
            # save the current size to a file
            if (newpkgSize > 0) :
                with open(pkgSizeFile, 'w') as the_size_file:
                    the_size_file.write(str(newpkgSize)+'\n')
            if (newshellSize > 0) :
                with open(shellSizeFile, 'w') as the_size_file:
                    the_size_file.write(str(newshellSize)+'\n')

def build_all_thirdparty(packagelist):
    '''
        build all third party lib
    '''
    third_party_env = os.environ.copy()
    third_party_env['CC'] = which('gcc')
    third_party_env['CXX'] = which('g++')
    for onePackage in packagelist:
        build_thirdparty(onePackage, third_party_env)


def PrintBuildHelp(common_env):
    '''
        Print the build help. testing framework will be a separate help
    '''
    hflag = GetOption('help')
    if hflag:
        print """
    ***********************************************************
    *                  GraphSQL Build Manual                  *
    ***********************************************************

    ========== End User Package ===============================
    'scons pkg'      to build the production program,
    'scons gudf_sdk' to build the graph UDF SDK (todo),

    ========== Dev build all ==================================
    'scons release' to build the full release version program,
    'scons debug' to build the full debug version program,

    ========== Dev build: Individual===========================
    ***** release *****
    'scons gse2_release' to build the release version of GSE 2.0,
    'scons cse_release' to build the release version of graphsql store,
    'scons gperun_release' to build the release version of gpe engine,
    'scons partition_release' to build the release version of partition,
    'scons analyticsrun_release' to build the release version of graph analytics.
    'scons gpelib_release' to build the release version of gpelib,
    'scons cselive_release' to build the release version of incremental store,
    ***** debug *****
    'scons gse2_debug' to build the debug version of GSE 2.0,
    'scons cse_debug' to build the debug version of graphsql store,
    'scons gperun_debug' to build the debug version of gpe engine,
    'scons partition_debug' to build the debug version of partition,
    'scons analyticsrun_debug' to build the debug version of graph analytics.
    'scons gpelib_debug' to build the debug version of gpelib,
    'scons cselive_debug' to build the debug version of incremental store,

    ========== Unit test build ================================
    'scons unit_debug' to build the debug version unit test,
    'scons unit | unit_release' to build the release version unit test

    ========== Testing Framework Invokation ===================
    'scons test' to run all tests
    'scons test [options]' to run specific level tests or with flags
    'scons test -h' to show test options and other help

    ========== Utility build ==================================
    'scons graph_generator' to build the release version of graph generator.

    ========== Source Code Document ===========================
    'scons docs' to build doxygen Document **

    ========== Other options: key=value =======================
    'verbose=0|1' 1 for verbose output. Default 0.
    'latest =1|0' 1 for latest package only. Default 1.
    'pg     =0|1' 1 for gprof build. Default 0.
    'cb     =0|1' 1 for customer build. Default 0.
    '-j n' for parallel build. Default 4."""

        if len(common_env['GSQLOPTIONS']) > 0:
            print """
    ========== Other options: --key value ====================="""
            for element in common_env['GSQLOPTIONS']:
                print element

        print """
    ----------- Notes -----------------------------------------
    ** Need to install doxygen and GraphViz under /opt/local/bin.
    E.g., on Mac OS X:
        sudo port install graphviz
        sudo port install doxygen

    ==========================================================="""
        sys.exit(0)

def AddNewCMDOption(common_env, newOptKey, newDest, newType, newMeta, newHelp, newDefault):
    new_opt = AddOption(
            newOptKey,
            dest=newDest,
            type=newType,
            nargs=1,
            action='store',
            metavar=newMeta,
            help=newHelp,
            default=newDefault)
    helpStr = ('    \''+newOptKey + ' '+ new_opt.metavar + '\'\t'
               + new_opt.help+ '. Default '+ new_opt.default)
    common_env.Append(GSQLOPTIONS=[helpStr])

def AddNewBoolOption(common_env, newOptKey, newDest, newaction, newHelp, newDefault):
    new_opt = AddOption(
            newOptKey,
            dest=newDest,
            action=('store_true' if newaction else'store_false'),
            help=newHelp,
            default=newDefault)
    helpStr = ('    \''+newOptKey + '\'\t'
               + new_opt.help+ ' Default is '+ str(new_opt.default))
    common_env.Append(GSQLOPTIONS=[helpStr])



def SetupCommonBuildEnv(common_env, verbose):
    '''
        setup the common build env
    '''
    # if not verbose, we will show color
    if not verbose:
        ColorEnv(common_env)

    ############################################################
    ##  Default parallelism (commandline -j will override this)#
    ############################################################
    num_cpu = int(os.environ.get('NUM_CPU', 4))
    SetOption('num_jobs', num_cpu)
    print "running with -j", GetOption('num_jobs')

    ############################################################
    ## 0.1 Define a common build environment                   #
    ## and then clone a release and a debug environment        #
    ############################################################

    common_env.Append(PYSYSPLATFORM=os.sys.platform)

    # generate the version information.
    if int(ARGUMENTS.get('latest', 1)):
        common_env.Append(BUILD_VERSION='bin')
    else:
        #use git branch/commit hash as build version.
        pkgbuildVersion= os.popen(
            "echo `git symbolic-ref HEAD 2> /dev/null | cut -b 12-`-`git log --pretty=format:%h -1`"
            ).read()
        pkgbuildVersion = pkgbuildVersion.rstrip('\n')
        common_env.Append(BUILD_VERSION=pkgbuildVersion)

    #add detailed version info (above plus date) to executable
    version_cmd = subprocess.Popen(['./gworkspace.sh','-c'], stdout=subprocess.PIPE)
    cppbuildVersion = version_cmd.stdout.read()
    cppbuildVersion = cppbuildVersion.replace("\n", ",").strip()
    cppbuildVersion ="\\\"" + cppbuildVersion + "\\\"";
    common_env.Append(CPPDEFINES={'BUILDVERSION':cppbuildVersion})

    # Show OS and also define OS_MACOSX for Mac OS X
    print "OS is " + common_env['PYSYSPLATFORM']
    if common_env['PYSYSPLATFORM'] == 'darwin':
        common_env.Append(CPPDEFINES={'OS_MACOSX':1})

    common_env['CC']=which('gcc')
    common_env['CXX']=which('g++')
    #-----------------------------------------------
    # 0.2: Common include paths and libs           |
    #-----------------------------------------------
    common_cpp_path=Split("""#/src/third_party
                             #/src/third_party/boost
                             #/src/third_party/yaml/usr/local/include
                             #/src/third_party/sparsehash/usr/local/include
                             #/src/third_party/glog/usr/local/include
                             #/src/third_party/jsoncpp/include
                             #/src/third_party/redis/deps/hiredis
                             #/src/third_party/zlib
                             #/src/third_party/zookeeper_c/include/zookeeper
                             #/src/core
                             #/src/core/gse
                             #/src/core/gpe
                             #/src/core/topology
                             #/src/utility
                             #/src/msg
                             #/src/msg/distributed
                   """)
    common_env.Append(CPPPATH=common_cpp_path)

    common_lib_files=Split("""
                        #/src/third_party/yaml/usr/local/lib/libyaml-cpp.a
                        #/src/third_party/boost/release/lib/libboost_date_time.a
                        #/src/third_party/boost/release/lib/libboost_serialization.a
                        #/src/third_party/boost/release/lib/libboost_wserialization.a
                        #/src/third_party/boost/release/lib/libboost_filesystem.a
                        #/src/third_party/boost/release/lib/libboost_system.a
                        #/src/third_party/boost/release/lib/libboost_program_options.a
                        #/src/third_party/boost/release/lib/libboost_thread.a
                        #/src/third_party/glog/usr/local/lib/libglog.a
                        #/src/third_party/gtest/libgtest.a
                        #/src/third_party/redis/deps/hiredis/libhiredis.a
                        #/src/third_party/zlib/libz.a
                        #/src/third_party/zookeeper_c/lib/libzookeeper_mt.a
                     """)
    common_libs =File(common_lib_files)
    common_env.Append(LIBS=[common_libs])

    # On Linux, we need to link to pthread and rt
    if not common_env['PYSYSPLATFORM'] == 'darwin':
        common_env.Append(LIBS = ['pthread','rt'])

    # add customer cppflags and link flags
    common_env.Append(CCFLAGS=GetOption('gcppflags'))
    common_env.Append(LINKFLAGS=GetOption('glinkflags'))
    # split the defines
    DefineStr = GetOption('gcppdefines')
    allDefine = DefineStr.split(',')
    for eachDefine in allDefine:
        common_env.Append(CPPDEFINES=[eachDefine])


def SetupDebugBuildEnv(debug___env, DEBUG___DIR):
    '''
        setup the debug build env. debug_env is a
        clone of common_env plus below settings.
    '''
    #debug___env.Append(CPPDEFINES=['DEBUG'])
    debug___env.VariantDir(DEBUG___DIR, 'src', duplicate=0)
    debug___env.Append(CCFLAGS = [ '-O0','-g3','-Wall','-c','-fmessage-length=0',
                                        '-Wno-unused','-fpermissive', '-fno-omit-frame-pointer'])

    ###############################################################
    #                                                             #
    # !!!! gpelib has to be first as it depends on other libs !!! #
    #                                                             #
    ###############################################################
    debug___lib_files=[ DEBUG___DIR + '/core/gpe/libgpelib.a',
                        DEBUG___DIR + '/msg/distributed/libdistlib.a',
                        DEBUG___DIR + '/third_party/linenoise/liblinenoise.a',
                        DEBUG___DIR + '/third_party/murmurhash/libmurmurhash.a',
                        DEBUG___DIR + '/third_party/jsoncpp/libjson.a',
                        DEBUG___DIR + '/third_party/snappy/libsnappy.a'
                      ]
    debug___libs =File(debug___lib_files)
    debug___env.Prepend(LIBS=[debug___libs])


def SetupReleaseBuildEnv(release_env, RELEASE_DIR):
    '''
        setup the release build env. release_env is a
        clone of common_env plus below settings.
    '''
    release_env.Append(CPPDEFINES=['RELEASE'])
    release_env.VariantDir(RELEASE_DIR, 'src', duplicate=0)
    release_env.Append(CCFLAGS = [ '-O3','-Wall','-c','-fmessage-length=0', 
                                    '-Wno-unused','-fpermissive', '-fno-omit-frame-pointer'])
    # Allowing gprof on Linux. Mac OS X doesn't support gprof so disable it.
    if int(ARGUMENTS.get('pg',0)) and not common_env['PYSYSPLATFORM'] == 'darwin':
        release_env.Append(CCFLAGS =['-pg'], LINKFLAGS=['-pg'])

    ###############################################################
    #                                                             # 
    # !!!! gpelib has to be first as it depends on other libs !!! #
    #                                                             #
    ###############################################################
    release_lib_files=[ RELEASE_DIR + '/core/gpe/libgpelib.a',
                        RELEASE_DIR + '/msg/distributed/libdistlib.a',
                        RELEASE_DIR + '/third_party/linenoise/liblinenoise.a',
                        RELEASE_DIR + '/third_party/murmurhash/libmurmurhash.a',
                        RELEASE_DIR + '/third_party/jsoncpp/libjson.a',
                        RELEASE_DIR + '/third_party/snappy/libsnappy.a'
                      ]
    release_libs =File(release_lib_files)
    release_env.Prepend(LIBS=[release_libs])

    # Allowing gprof on Linux. Mac OS X doesn't support gprof so disable it.
    if int(ARGUMENTS.get('pg',0)) and not release_env['PYSYSPLATFORM'] == 'darwin':
        release_env.Append(CCFLAGS =['-pg'], LINKFLAGS=['-pg'])

def SetupSharedBuildEnv(release_env, RELEASE_DIR):
    '''
        setup the release build env. release_env is a
        clone of common_env plus below settings.
    '''
    release_env.Append(CPPDEFINES=['RELEASE'])
    release_env.VariantDir(RELEASE_DIR, 'src', duplicate=0)
    release_env.Append(CCFLAGS = [ '-O3','-Wall','-c','-fmessage-length=0', 
                                    '-Wno-unused','-fpermissive','-fPIC'])
    # Allowing gprof on Linux. Mac OS X doesn't support gprof so disable it.
    if int(ARGUMENTS.get('pg',0)) and not common_env['PYSYSPLATFORM'] == 'darwin':
        release_env.Append(CCFLAGS =['-pg'], LINKFLAGS=['-pg'])

    ###############################################################
    #                                                             # 
    # !!!! gpelib has to be first as it depends on other libs !!! #
    #                                                             #
    ###############################################################
    release_lib_files=[ RELEASE_DIR + '/core/gpe/libgpelib-sh.a',
                        RELEASE_DIR + '/msg/distributed/libdistlib-sh.a',
                        RELEASE_DIR + '/third_party/linenoise/liblinenoise-sh.a',
                        RELEASE_DIR + '/third_party/murmurhash/libmurmurhash-sh.a',
                        RELEASE_DIR + '/third_party/jsoncpp/libjson-sh.a',
                        RELEASE_DIR + '/third_party/snappy/libsnappy-sh.a'
                      ]
    release_libs =File(release_lib_files)
    release_env.Prepend(LIBS=[release_libs])

    # Allowing gprof on Linux. Mac OS X doesn't support gprof so disable it.
    if int(ARGUMENTS.get('pg',0)) and not release_env['PYSYSPLATFORM'] == 'darwin':
        release_env.Append(CCFLAGS =['-pg'], LINKFLAGS=['-pg'])
