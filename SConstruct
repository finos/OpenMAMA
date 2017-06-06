import os,re
import sys
import posixpath,string
from custom_utils import *
from SCons.Script import *
import subprocess,time,shlex

from config import ParseIni

host = getHostInfo()

if host['os'] == 'Windows':
    buildWrapper = Windows()
if host['os'] == 'Linux':
    buildWrapper = Linux()
if host['os'] == 'Darwin':
    buildWrapper = Darwin()

parseconfig = ParseIni()

## Parse Available Products
#
# Go through the scons.ini and pull the products that are valid choices from it
ValidProducts = parseconfig.getProducts()

## Version Definitions
#
# declare the versions for each product
VERSIONS = get_project_versions( '.' )

## Get Command Line Options
opts = get_command_line_opts( host, ValidProducts, VERSIONS )

env = buildWrapper.create( opts )

opts.Save('omama.conf', env)

Help(opts.GenerateHelpText(env, sort=True))

if env['jobs'] == 'n':
    env['jobs'] = host['cpus']

SetOption('num_jobs', env['jobs'])

if (env['prefix'] == '#wombat_products_%s' % (VERSIONS['mama']['releaseString'])
    or  env['prefix'] == '#openmama_install_%s' % (VERSIONS['mama']['releaseString'])):
    env.LogDefaultValue( 'Prefix',env['prefix'] )

if ('wombat_products_%s' % (VERSIONS['mama']['releaseString']) not in env['prefix']
    and os.path.isdir ('site_scons/enterprise')):
    env['prefix'] = posixpath.join( env['prefix'], 'wombat_products_%s' % (VERSIONS['mama']['releaseString']) )
    env.LogUserValue( 'Prefix',env['prefix'] )

env.LogConfig( 'Object Directory', env['blddir'], '#objdir' )

if env.has_key('cache_dir'):
    CacheDir(env['cache_dir'])
    env.LogUserValue( 'SCons Cache', env['cache_dir'] )

# If the timestamp on the file is the same as before then don't bother checking
# the contents of the file by summing
env.Decider('MD5-timestamp')

# From the website:
#
# By default SCons will calculate the MD5 checksum of every source file in your
# build each time it is run, and will only cache the checksum after the file is
# 2 days old. This default of 2 days is to protect from clock skew from NFS or
# revision control systems. You can tweak this delay using --max-drift=SECONDS
# where SECONDS is some number of seconds. Decreasing SECONDS can improve build
# speed by eliminating superfluous MD5 checksum calculations. Instead of
# specifying this on the command line each run, you can set this option inside
# your SConstruct or SConscript file using "SetOption('max_drift', SECONDS)".
env.SetOption('max_drift', 1)

env['versions'] = VERSIONS

prefix          = env['prefix']
blddir          = env['blddir']
product         = env['product']

env['incdir']   =  '$prefix/include'
env['libdir']   =  '$prefix/lib'
env['bindir']   =  '$prefix/bin'

env['host']     = host

StaticLibs = {}
Jars = {}
# Media is used for the management of files which do not fall within the Windows
# dynamic,static etc. buildtypes e.g. example app src code. If installed without
# any catches SCons will report errors concerning multiple build environments for
# output
Media = {}
Lock = {}
Export('StaticLibs')
Export('Jars')
Export('Media')
Export('Lock')

env['tarball'] = buildWrapper.getReleaseTarball(env)

env['TARFLAGS'] = '-c -z'
env['TARSUFFIX'] = 'tgz'
env['TOPLEVEL'] = Dir('#').abspath

## Build Modules
#
# Setup the modules that are to be built, and then call each of the appropriate
# SConscript files.
modules = parseconfig.getProductModules(product)

if 'wmw' in env['middleware']:
	modules.append('wombatmw/c_cpp')

env['modules'] = modules

if not env.GetOption('help'):
    # # Build Environment Setup
    #
    # Setup the appropriate flags for each of the build environments in this section
    # using the wrapper class

    buildWrapper.configure(env)

    log_api_config(env)

    buildWrapper.build(env, modules)

    #if env['with_docs'] == True:
    #    env.Command( '$prefix/VERSIONS', [], write_versions_file )
    #    env.Command( '$prefix/RELEASE_NOTES/',[], generate_release_notes )

    # # Clean Target
    #
    # Define the Clean target. Both folders will be removed as part of the Clean
    # operation
    Clean( '.', [env['prefix'], env['blddir']] )

    # # Variables Save
    #
    # Saves all non-default values within the Variables class to the omama.conf

    if env.get('test',None) == True:
        Test.run( env )

    Alias ('install', env.Install('$prefix', ["LICENSE-3RD-PARTY.txt", "LICENSE.md", "README.md"]))

