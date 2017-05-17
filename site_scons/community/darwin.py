import os, posixpath, platform
from custom_builders import *
from logger import Logger
from log_helper import *
from SCons.Script import *

from jni_tools import *

class Darwin:
    # Get the Release Tarball using the nameReleaseTarball Helper Script
    def getReleaseTarball(self,env):
        return 'openmama_2.3.0.tgz'
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

        if os.environ.get('DYLD_LIBRARY_PATH',None) != None:
            os_env['DYLD_LIBRARY_PATH'] = os.environ['DYLD_LIBRARY_PATH']

        # Compliler default for MacOSX should be clang(++)
        compiler = optsEnv['compiler']
        if compiler == 'clang-analyzer':
            tools.append( 'scan-build')
        else:
            tools.append('clang')
            tools.append('clang++')

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

        # Get the OSX version and set sysroot / min version
        if env['osx_version'] == "current":
            osxversion = self.getMacOSXVersion()
        else:
            osxversion = env['osx_version']
        sysroot = '/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX'+osxversion+'.sdk'
        minversion = '-mmacosx-version-min='+osxversion

        env.Log("Building for Mac OSX " + osxversion)

        #env.Append( CPPDEFINES = ['HAVE_CONFIG_H'] )
        env.Append( CCFLAGS = ['-fcolor-diagnostics','-arch','x86_64',minversion,'-isysroot',sysroot,'-g','-O2','-pedantic-errors'])
        env.Append( CFLAGS = ['-std=gnu99','-Wmissing-prototypes','-Wstrict-prototypes','-Wno-int-conversion'])
        env.Append( CXXFLAGS = ['-Wall','-Wno-long-long','-stdlib=libc++'] )
        env.Append( CPPPATH = ['/usr/include','/usr/local/include'] )
        env.Append( LIBPATH = ['/usr/local/lib'] )
        env.Append( LINKFLAGS = ['-stdlib=libc++','-arch','x86_64',minversion,'-isysroot',sysroot] )

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

        if os.environ.has_key('CPPPATH'):
            print 'Setting User defined CPPPATH: ' + os.environ['CPPPATH']
            env.Append(CPPPATH = Split( os.environ['CPPPATH'] ) )

        if env['with_unittest'] == True:
            if not Dir(env['gtest_home']).exists():
                print 'GTest Home not found: ' + env['gtest_home']
                Exit(0)

        if env['apr_home'] == True:
            if not Dir(env['apr_home']).exists():
                print 'Apache APR Home not found: ' + env['apr_home']
                Exit(0)

    def build(self, env, modules):
        for m in modules:
            vdir = os.path.join('$blddir', m)
            env.SConscript('%s/SConscript' % m, 'env',
                           variant_dir=vdir, duplicate=0)

    def getMacOSXVersion(self):
        version, _, _ = platform.mac_ver()
        version = (str)('.'.join(version.split('.')[:2]))
        return version;
