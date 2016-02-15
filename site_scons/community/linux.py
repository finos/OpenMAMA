import os, posixpath
from custom_builders import *
from logger import Logger
from log_helper import *
from SCons.Script import *

from jni_tools import *

class Linux:
    # Get the Release Tarball using the nameReleaseTarball Helper Script
    def getReleaseTarball(self,env):
        return 'openmama_2.2.0.tgz'
    def create( self, opts ):
        # It is necessary to put opts in there own Env to extract the values passed
        # as the Variables class does not allow you to access values. Some Construction
        # variables must be defined when the Environment is created e.g. MSVC_VERSION
        # setting these after Environment creation has no effect
        #
        # Likewise the JAVA_HOME must be set on the path at environment creation otherwise
        # it will not recognise the tools
        optsEnv = Environment()
        opts.Update( optsEnv )

        tools = [ 'default' ]

        logger = Logger(optsEnv)

        os_env = {}

        if os.environ.get('LD_LIBRARY_PATH',None) != None:
            os_env['LD_LIBRARY_PATH'] = os.environ['LD_LIBRARY_PATH']

        compiler = optsEnv['compiler']
        if compiler != 'default':
            if compiler == 'clang':
                tools.append( 'clang' )
                tools.append( 'clang++' )
            if compiler == 'clang-analyzer':
                if 'scan-build' not in os.environ['_'] and not GetOption('clean'):
                    print 'If you wish to run with the static analyzer, please ensure ' \
                            'you execute scons within the scan-build tool.'
                    print '\tscan-build <scan-build-arguments> scons <scons-arguments>'
                    exit (1)
                tools.append( 'scan-build')

        if optsEnv['with_docs'] == True:
            tools.append( 'doxygen' )
            tools.append( 'javadoc' )

        if optsEnv['product'] == 'mamdajni' or optsEnv['product'] == 'mamajni' or optsEnv['product'] == 'mamdajava':
            if not optsEnv.get('java_home'):
                print 'java_home has not been set, exiting...'
                Exit(1)

            os_env['PATH']      = '%s:%s/bin' % (os.environ['PATH'], optsEnv['java_home'])
            os_env['JAVA_HOME'] = optsEnv['java_home']

            env = Environment(ENV=os_env,
                    tools = tools,
                    toolpath = ['site_scons/site_tools'])

            #ConfigureJNI searches os.env for java_home not env['ENV']['JAVA_HOME'] 
            #This is needed if set on cmd line via scons java_home=/path/to/java
            os.environ['JAVA_HOME'] = optsEnv['java_home']
            if not ConfigureJNI(env):
                print 'Java Native Interface is required...Exiting'
                Exit(1)

            env['JAVAH'] = 'javah'

        else:
            os_env['PATH'] = os.environ['PATH']
            env = Environment(ENV=os_env,
                        tools = tools, 
                        toolpath = ['site_scons/site_tools'])

        env['SPAWN'] = logger.log_output
        env['PRINT_CMD_LINE_FUNC'] = logger.log_command

        env['Logger'] = logger

        opts.Update( env )

        AddMethod( env, Log )
        AddMethod( env, LogInfo )
        AddMethod( env, LogWarning )
        AddMethod( env, LogDebug )
        AddMethod( env, LogError )
        AddMethod( env, user_value,     'LogUserValue' )
        AddMethod( env, default_value,  'LogDefaultValue' )
        AddMethod( env, log_config,     'LogConfig' )

        env.AddMethod(symlinkFiles, "SymLinkFiles")

        env.Log("Community build")
        unknown = opts.UnknownVariables()
        if unknown:
            env.LogError("Unknown variables:{}".format(unknown.keys() ))
            Exit(1)

        return( env )

    # # Setup the Linux Build Environment
    #
    # Configures all the necessary environment variables for Linux
    def configure(self, env ):
        env.Append( CPPDEFINES = ['_GNU_SOURCE'] )
        env.Append( CCFLAGS = ['-g','-O2','-pedantic-errors'] )
        env.Append( CFLAGS = ['-std=gnu99','-Wmissing-prototypes', '-Wstrict-prototypes'] )
        env.Append( CXXFLAGS = ['-Wall','-Wno-long-long'] )

        if env['target_arch'] == 'x86':
            env.Append(CCFLAGS  = ['-m32'])
            env.Append(CFLAGS   = ['-m32'])
            env.Append(LINKFLAGS = ['-m32'])

        if os.environ.has_key('CC'):
            print 'Setting User defined CC: ' + os.environ['CC']
            env['CC'] = os.environ['CC']

        if os.environ.has_key('CXX'):
            print 'Setting User defined CXX: ' + os.environ['CXX']
            env['CXX'] = os.environ['CXX']

        if os.environ.has_key('CCFLAGS'):
            print 'Setting User defined CCFLAGS: ' + os.environ['CCFLAGS']
            env.Append( CCFLAGS = Split( os.environ['CCFLAGS'] ) )

        if os.environ.has_key('CXXFLAGS'):
            print 'Setting User defined CXXFLAGS: ' + os.environ['CXXFLAGS']
            env.Append( CXXFLAGS = Split( os.environ['CXXFLAGS'] ) )

        if os.environ.has_key('LIBPATH'):
            print 'Setting User defined LIBPATH: ' + os.environ['LIBPATH']
            env.Append(LIBPATH = Split( os.environ['LIBPATH'] ) )

        if os.environ.has_key('LINKFLAGS'):
            print 'Setting User defined LINKFLAGS: ' + os.environ['LINKFLAGS']
            env.Append(LINKFLAGS = Split( os.environ['LINKFLAGS'] ) )

        if env['with_unittest'] == True:
            if not Dir(env['gtest_home']).exists():
                print 'GTest Home not found: ' + env['gtest_home']
                Exit(0)

    def build(self, env, modules):
        for m in modules:
            vdir = os.path.join('$blddir', m)
            env.SConscript('%s/SConscript' % m, 'env',
                           variant_dir=vdir, duplicate=0)
