from SCons.Script import *

## Command Line Variables
#
# Setup all of the command line variables across all of the products and
# platforms. NOTE: if a path is configurable and will be created in the
# build process then the validation MUST be PathAccept
def get_command_line_opts( host, products, VERSIONS ):
    opts = Variables('omama.conf')
    opts.format = '\n%s: %s\n Default: %s [ %s ]\n'

    opts.AddVariables(
       # Must be #install by default, otherwise when it comes to cleaning the
       # install folder, can remove whole tree
       PathVariable('prefix', 'Installation prefix', '#openmama_install_%s' % (VERSIONS['mama']['releaseString']),
                    PathVariable.PathAccept),
       PathVariable('blddir', 'Object directory', '#objdir',
                    PathVariable.PathAccept),
       PathVariable('java_home', 'JAVA Home folder', os.environ.get('JAVA_HOME',None) , PathVariable.PathAccept),
       PathVariable('logfile', 'Output Log File', 'scons.log', PathVariable.PathAccept),
       BoolVariable('verbose','Whether to print verbose output',True),
       BoolVariable('package','Whether to tar up the installation directory',False),
       BoolVariable('with_docs','Build with documentation',False),
       BoolVariable('with_unittest','Build with gunit tests',False),
       BoolVariable('with_testtools','Build with test tools',False),
       BoolVariable('with_examples','Build with test tools',True),
       PathVariable('oea_home','Path to oea entitlements home',None, PathVariable.PathIsDir),
       ListVariable('entitlements', 'List of entitlements libraries to enforce e.g. \'oea\' (NOTE: 1st in list the default entitlements library to use.)', 'noop',
                     names = ['oea','noop'] ),
       BoolVariable('debug', 'Build without compiler optimisation.',False),
       PathVariable('gtest_home','Path to Google Test home',None, PathVariable.PathIsDir),
       PathVariable('junit_home','Path to Junit home',None, PathVariable.PathIsDir),
       ListVariable('middleware','Middleware(s) to be compiled in', 'qpid', names = ['qpid'] ),
       ('jobs', 'Number of scons threads to spawn, if n is passed the number of availabe cores is calculated and used', '1'),

    )

    if host['os'] == 'Windows':
        env = Environment()
        opts.AddVariables(
            ListVariable( 'buildtype', 'Windows Build type e.g dynamic', 'all', names = ['dynamic','dynamic-debug','static','static-debug'] ),
            PathVariable('qpid_home', 'Path to QPID Proton Libraries',
                          'c:\\proton', PathVariable.PathAccept),
            EnumVariable('vsver','Visual Studio Version to use', env['MSVC_VERSION'],
                allowed_values=('8.0','9.0','10.0','11.0','12.0', '14.0')),
            EnumVariable('product', 'Product to be built', 'mamda',
                     allowed_values=( products )),
            EnumVariable('dotnet_version', 'Dotnet Version used to determine framework directory', '2.0',
                     allowed_values=('1.0','2.0', '4.0')),
            PathVariable('dotnet_framework', 'Path to desired dotnet framework', None,
                     PathVariable.PathIsDir),
            PathVariable('nunit_home', 'Path to Nunit home', None,
                     PathVariable.PathIsDir),
            PathVariable('libevent_home', 'Path to libevent Libraries',
                          'c:\\libevent', PathVariable.PathAccept),
            EnumVariable('target_arch', 'Specifies if the build should target 32 or 64 bit architectures.',
                          host['arch'], allowed_values=['x86', 'x86_64']),
            EnumVariable( 'compiler', 'Compiler to use for building OpenMAMA',
                          'default', allowed_values=('default', 'gcc', 'clang', 'clang-analyzer')),
        )

    if host['os'] == 'Linux':
        opts.AddVariables(
            PathVariable('qpid_home','Path to QPID Proton Libraries',
                '/usr/local/', PathVariable.PathIsDir),
            PathVariable('cache_dir','Path to object cache', None, PathVariable.PathIsDir),
            EnumVariable('product', 'Product to be built', 'mamda',
                         #mamda all is a windows only build
                         allowed_values=( [ x for x in products if x != "mamdaall" ] )),
            PathVariable('libevent_home', 'Path to libevent Libraries',
                          '/usr/', PathVariable.PathAccept),
            EnumVariable('target_arch', 'Specifies if the build should target 32 or 64 bit architectures.',
                          host['arch'], allowed_values=['i386', 'i586', 'i686', 'x86', 'x86_64']),
            EnumVariable( 'compiler', 'Compiler to use for building OpenMAMA',
                         'default', allowed_values=('default', 'gcc', 'clang', 'clang-analyzer')),
        )

    if host['os'] == 'Darwin':
        opts.AddVariables(
            PathVariable('qpid_home','Path to QPID Proton Libraries',
                '/usr/local/', PathVariable.PathIsDir),
            PathVariable('cache_dir','Path to object cache', None, PathVariable.PathIsDir),
            EnumVariable('product', 'Product to be built', 'mamda',
                         #mamda all is a windows only build
                         allowed_values=( [ x for x in products if x != "mamdaall" ] )),
            EnumVariable( 'compiler', 'Compiler to use for building OpenMAMA',
                         'default', allowed_values=('default', 'clang', 'clang-analyzer')),
            EnumVariable('osx_version', 'OS X Version to target build at', 'current',
                         allowed_values=('current','10.8','10.9','10.10','10.11')),
            )

    return opts
