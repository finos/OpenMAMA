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
       PathVariable('prefix', 'Installation prefix', '#wombat_products_%s' % (VERSIONS['mama']['releaseString']),
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
       BoolVariable('entitled','Whether the build is entitled or unentitled',False),
       PathVariable('gtest_home','Path to Google Test home',None, PathVariable.PathIsDir),
       ListVariable('middleware','Middleware(s) to be compiled in', 'avis', names = ['avis'] ),

    )

    if host['os'] == 'Windows':
        opts.AddVariables(
            ListVariable( 'buildtype', 'Windows Build type e.g dynamic', 'all', names = ['dynamic','dynamic-debug','static','static-debug'] ),
            PathVariable('avis_home', 'Path to Avis',
                          'c:\\avis', PathVariable.PathAccept),
            EnumVariable('vsver','Visual Studio Version to use', '10.0',
                allowed_values=('8.0','9.0','10.0')),
            EnumVariable('product', 'Product to be built', 'mamda',
                     allowed_values=( products )),
            EnumVariable('dotnet_version', 'Dotnet Version used to determine framework directory', '2.0',
                     allowed_values=('1.0','2.0', '4.0')),
            PathVariable('dotnet_framework', 'Path to desired dotnet framework', None,
                     PathVariable.PathIsDir),
        )

    if host['os'] == 'Linux':
        opts.AddVariables(
            PathVariable('avis_home','Path to Avis', '/usr/local/', PathVariable.PathIsDir),
            PathVariable('cache_dir','Path to object cache', None, PathVariable.PathIsDir),
            EnumVariable('product', 'Product to be built', 'mamda',
                         #mamda all is a windows only build
                         allowed_values=( [ x for x in products if x != "mamdaall" ] )),
        )

    return opts
