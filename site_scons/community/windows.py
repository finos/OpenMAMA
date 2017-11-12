import os, subprocess,posixpath,string, re,ctypes
import platform
from logger import Logger
from log_helper import *
from custom_builders import *
from SCons.Script import *

from config import ParseIni

import subprocess, shlex

from jni_tools import *

class Windows:
    # Get the Release Tarball using the nameReleaseTarball Helper Script
    def getReleaseTarball(self,env):
        return 'openmama_2.2.0.tgz'
    def create( selv, opts ):
        optsEnv = Environment()
        opts.Update( optsEnv )

        tools = [ 'default' ]

        logger = Logger(optsEnv)

        if optsEnv['with_docs'] == True:
            tools.append( 'doxygen' )
            tools.append( 'javadoc' )

        if optsEnv['product'] == 'mamdaall':
            tools.append( 'csharp' )

        programfiles_x86 = os.environ['ProgramFiles(x86)']
        # Select which default program files based directory to use
        if optsEnv['target_arch'] == 'x86':
            programfiles = programfiles_x86
        else:
            programfiles = os.environ['ProgramW6432']

        if optsEnv['product'] == 'mamdajni' or optsEnv['product'] == 'mamajni' or optsEnv['product'] == 'mamdaall':
            if not optsEnv.get('java_home'):
                print 'java_home has not been set, exiting...'
                Exit(1)

            tools.append('javah')

            env = Environment(ENV={
                'JAVA_HOME': '%s' % (optsEnv['java_home']),
                'PATH': '%s:%s\\bin' % (os.environ['PATH'], optsEnv['java_home']),
                },
                tools = tools,
                MSVC_VERSION = optsEnv['vsver'],
                MSVS_VERSION = optsEnv['vsver'],
                TARGET_ARCH = optsEnv['target_arch'])

            #ConfigureJNI searches os.env for java_home not env['ENV']['JAVA_HOME']
            #This is needed if set on cmd line via scons java_home=/path/to/java
            os.environ['JAVA_HOME'] = optsEnv['java_home']
            if not ConfigureJNI(env):
                print 'Java Native Interface is required...Exiting'
                Exit(1)

            env['JAVAH'] = 'javah'

        else:
            env = Environment(ENV={'PATH': '%s' % (os.environ['PATH'])}, MSVC_VERSION = optsEnv['vsver'], MSVS_VERSION = optsEnv['vsver'], tools = tools, TARGET_ARCH = optsEnv['target_arch'])

        if 'qpid' in optsEnv['middleware']:
            if optsEnv.has_key('qpid_home'):
                env['qpid_home'] = optsEnv['qpid_home']
            else:
                env['qpid_home'] = "%s/Proton" % programfiles

            if optsEnv.has_key('libevent_home'):
                env['libevent_home'] = optsEnv['libevent_home']
            else:
                env['libevent_home'] = "%s/libevent" % programfiles

            if not posixpath.exists(env['libevent_home']):
                print 'ERROR: Libevent Home (%s) (required for qpid) must exist' % env['libevent_home']
                Exit(1)
            if not posixpath.exists(env['qpid_home']):
                print 'ERROR: Qpid Home (%s) must exist' % env['qpid_home']
                Exit(1)

        if optsEnv.has_key('apr_home'):
            env['apr_home'] = optsEnv['apr_home']
        else:
            env['apr_home'] = "%s/APR" % programfiles

        if not posixpath.exists(env['apr_home']):
            print 'ERROR: Apache APR Home (%s) must exist' % env['apr_home']
            Exit(1)

        if optsEnv['with_unittest'] == True:
            if optsEnv.has_key('gtest_home'):
                env['gtest_home'] = optsEnv['gtest_home']
            else:
                env['gtest_home'] = "%s/googletest-distribution" % programfiles

            if not posixpath.exists(env['gtest_home']):
                print 'ERROR: GTest Home (%s) must exist' % env['gtest_home'] 
                Exit(1)

            if optsEnv['product'] == 'mamdaall':
                if optsEnv.has_key('nunit_home'):
                    env['nunit_home'] = optsEnv['nunit_home'] 
                else:
                    env['nunit_home'] = "%s/NUnit 2.6.4" % programfiles_x86

                if not posixpath.exists(env['nunit_home']):
                    print 'ERROR: Nunit Home (%s) must exist' % env['nunit_home']
                    Exit(1)
        try:
            subprocess.check_call("flex --version", shell=True, stdout=None, stderr=None)
        except:
            print "Could not execute flex - is it in your environment PATH?"

        env['SPAWN'] = logger.log_output
        env['PRINT_CMD_LINE_FUNC'] = logger.log_command

        env['Logger'] = logger

        env['ENV']['TMP'] = os.environ['TMP']

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
        env.AddMethod(install_library, "InstallLibrary")

        env.Log("Community build")
        unknown = opts.UnknownVariables()
        if unknown:
            env.LogError("Unknown variables:{}".format(unknown.keys() ))
            Exit(1)

        if os.path.isdir(env.get('shared_directory','/NoSharedDirSet')):
            env.LogConfig( 'Shared Directory', env.get('shared_directory',None), env.get('shared_directory',None) )
        else:
            env.LogError("shared_dir %s does not exist" % env.get('shared_directory',None))

        #Cast these for use in dir names
        env['vsver_maj'] = int(env['vsver'].split('.')[0])
        env['vsver_min'] = int(env['vsver'].split('.')[1])
        return( env )

    def build(self, env, modules):
        dynamic       = env.Clone()
        dynamic_debug = env.Clone()
        static        = env.Clone()
        static_debug  = env.Clone()
        media = env.Clone()

        media.Append(
            build       = 'media',
            )

        dynamic.Append(
            CCFLAGS     = ['/MD','/Ox'],
            build       = 'dynamic',
            suffix      = 'md',
            )

        dynamic_debug.Append(
            CCFLAGS     = ['/MDd'],
            build       = 'dynamic-debug',
            suffix      = 'mdd',
            )

        static.Append(
            CCFLAGS     = ['/MT','/Ox'],
            build       = 'static',
            suffix      = 'mt',
            )

        static_debug.Append(
            CCFLAGS     = ['/MTd'],
            build       = 'static-debug',
            suffix      = 'mtd',
            )

        builds = []
        if 'all' in env['buildtype']:
            builds = [ dynamic, static, dynamic_debug, static_debug ]
        else:
            if 'dynamic' in env['buildtype']:
                builds.append( dynamic )
            if 'dynamic-debug' in env['buildtype']:
                builds.append( dynamic_debug )
            if 'static' in env['buildtype']:
                builds.append( static )
            if 'static-debug' in env['buildtype']:
                builds.append( static_debug )

        example_src_handled = False
        for env in builds:
            print 'Building: %s' % (env['build'])

            if env['build'].startswith('dynamic') and not example_src_handled:
                example_src_handled = True
                env['inc_example_src'] = True

            if 'debug' in env['build']:
                env['win_type_path'] = 'Debug'
            else:
                env['win_type_path'] = 'Release'

            env['lib_dep_prefix'] = os.path.join(env['win_arch_path'], env['win_type_path'])

            env['bindir'] = '$prefix/bin/$build'
            env['libdir'] = '$prefix/lib/$build'

            env.Append( LIBPATH = '$libdir' )
            build = 0

            # JNI and .NET are only built for dynamic and dynamic debug
            # build variants. Therefore a build flag is used to signal
            for m in modules:
                if ( m == 'mama/jni' or 'mama/dotnet' ):
                    if ( env['build'] == 'dynamic' or 'dynamic-debug' ):
                        build = 1
                    else:
                        build = 0
                else:
                    build = 1

                if build == 1:
                    vdir = posixpath.join( env['blddir'], '%s/%s' % (env['build'], m) )
                    env.SConscript('%s/SConscript.win' % m, 'env',
                                   variant_dir=vdir,
                                   duplicate=0)

    ## Setup the Windows Build Environment
    #
    # Configures all of the appropriate environment variables for windows.
    def configure(self, env ):
        env.Append( CPPDEFINES = ['WIN32'] )
        env.Append( CCFLAGS = ['/EHsc','/GR','/FIwombat\\targetsxs.h'] )
        env.Append( LINKFLAGS =['/MANIFEST'] )
        env.Append( CCPDBFLAGS = '/Fd${TARGET}.pdb' )
        env.Append( PDB = '${TARGET.base}.pdb')

        # Architecture specific setup
        if env['target_arch'] == 'x86_64':
            env['bit_suffix'] = '_x64'
            env['win_arch_path'] = 'x64'
        else:
            env['bit_suffix'] = ''
            env['win_arch_path'] = ''

        if env['vsver'] > 7:
            env.Append( CCFLAGS = [ '/Z7','/MP' ])
        else:
            env.Append( CCFLAGS = [ '/Zi' ])

        # Entitlement setup
        if 'oea' in env['entitlements']:
            ( oea_major, oea_minor, oea_release ) = env['oea_version'].split('.')

            env.Append(
                CPPDEFINES = ['OEA_MAJVERSION=%s' % ( oea_major ), 'OEA_MINVERSION=%s' % (oea_minor)],
                CPPPATH = [ posixpath.join( env['oea_home'], 'include/oea') ],
            )

        # Expat Setup
        #env['expathome'] = posixpath.join( env['shared_directory'], 'Expat/Expat-%s/Source' % ( env['expat_version'] ) )
        #env['expatobj'] = posixpath.join( env['expat_home'],'lib/vc%s%s/release_static/*.obj' % ( env['vsver'], env['bit_suffix'] ) )

        if env['product'] == 'mamdaall':

            if not env.has_key('dotnet_framework'):
                print 'Calculating dotnet_framework'
                if env['target_arch'] == 'x86_64':
                    framework = 'Framework64'
                else:
                    framework = 'Framework'

                framework_dir = posixpath.join( 'c:/windows/Microsoft.NET/',framework )

                if env['dotnet_version'] == '1.0':
                    dotnetver = 'v1.1.4322'

                elif env['dotnet_version'] == '2.0':
                    dotnetver = 'v2.0.50727'

                elif env['dotnet_version'] == '4.0':
                    dotnetver = 'v4.0.30319'

                env['dotnet_framework'] = posixpath.join( framework_dir, dotnetver )

            if not posixpath.exists(env['dotnet_framework']):
                print 'ERROR: Dotnet framework dir: %s not found' % (env['dotnet_framework'])
                Exit(1)
