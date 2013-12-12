import os,sys
import SCons.Action
import SCons.Builder
import SCons.Util
from SCons.Script import *
from SCons.Node.FS import _my_normcase
from SCons.Tool.JavaCommon import parse_java_file

def walkDirs(path):
    """helper function to get a list of all subdirectories"""

    def addDirs(pathlist, dirname, names):
        """internal function to pass to os.path.walk"""

        for n in names:
            f = os.path.join(dirname, n)
            if os.path.isdir(f):
                pathlist.append(f)

    pathlist = [path]
    os.path.walk(path, addDirs, pathlist)
    return pathlist


def ConfigureJNI(env):
    """Configure the given environment for compiling Java Native Interface
       c or c++ language files."""

    if not env.get('JAVAC'):
        print 'The Java compiler must be installed and in the current path.'
        return 0

    # first look for a shell variable called JAVA_HOME

    java_base = os.environ.get('JAVA_HOME')
    if not java_base:
        if sys.platform == 'darwin':

            # Apple's OS X has its own special java base directory

            java_base = '/System/Library/Frameworks/JavaVM.framework'
        else:

            # Search for the java compiler

            print 'JAVA_HOME environment variable is not set. Searching for java... ',
            jcdir = os.path.dirname(env.WhereIs('javac'))
            if not jcdir:
                print 'not found.'
                return 0

            # assuming the compiler found is in some directory like
            # /usr/jdkX.X/bin/javac, java's home directory is /usr/jdkX.X

            java_base = os.path.join(jcdir, '..')
            print 'found.'

    if sys.platform == 'cygwin':

        # Cygwin and Sun Java have different ideas of how path names
        # are defined. Use cygpath to convert the windows path to
        # a cygwin path. i.e. C:\jdkX.X to /cygdrive/c/jdkX.X

        java_base = os.popen("cygpath -up '" + java_base + "'"
                             ).read().replace('\n', '')

    if sys.platform == 'darwin':

        # Apple does not use Sun's naming convention

        java_headers = [os.path.join(java_base, 'Headers')]
        java_libs = [os.path.join(java_base, 'Libraries')]
    else:

        # windows and linux

        java_headers = [os.path.join(java_base, 'include')]
        java_libs = [os.path.join(java_base, 'lib')]

        # Sun's windows and linux JDKs keep system-specific header
        # files in a sub-directory of include

        if java_base == '/usr' or java_base == '/usr/local':

            # too many possible subdirectories. Just use defaults

            java_headers.append(os.path.join(java_headers[0], 'win32'))
            java_headers.append(os.path.join(java_headers[0], 'linux'))
            java_headers.append(os.path.join(java_headers[0], 'solaris'
                                ))
        else:

            # add all subdirs of 'include'. The system specific headers
            # should be in there somewhere

            java_headers = walkDirs(java_headers[0])

    # add Java's include and lib directory to the environment

    env.Append(CPPPATH=java_headers)
    env.Append(LIBPATH=java_libs)

    # add any special platform-specific compilation or linking flags

    if sys.platform == 'darwin':
        env.Append(SHLINKFLAGS='-dynamiclib -framework JavaVM')
        env['SHLIBSUFFIX'] = '.jnilib'
    elif sys.platform == 'cygwin':
        env.Append(CCFLAGS='-mno-cygwin')
        env.Append(SHLINKFLAGS='-mno-cygwin -Wl,--kill-at')

    # Add extra potentially useful environment variables

    env['JAVA_HOME'] = java_base
    env['JNI_CPPPATH'] = java_headers
    env['JNI_LIBPATH'] = java_libs
    return 1

def classname(path):
    """Turn a string (path name) into a Java class name."""
    return os.path.normpath(path).replace(os.sep, '.')

def find_java_files(env, arg, dirpath, filenames):
    java_suffix = env.get('JAVASUFFIX', '.java')
    js = _my_normcase(java_suffix)

    java_files = sorted([n for n in filenames
                           if _my_normcase(n).endswith(js)])
    mydir = dirnode.Dir(dirpath)
    java_paths = [mydir.File(f) for f in java_files]
    for jp in java_paths:
         arg[jp] = True

## Replacement Java Class Emitter
#
# The default emitter for the Java builder walks the full directory tree
# this causes problems when you do not want to build all files below a 
# folder, e.g. in mamda. This emitter does not recurse through directories.

def emit_java_classes_norecurse(target, source, env):
    """Create and return lists of source java files
    and their corresponding target class files.
    """
    java_suffix = env.get('JAVASUFFIX', '.java')
    class_suffix = env.get('JAVACLASSSUFFIX', '.class')

    target[0].must_be_same(SCons.Node.FS.Dir)
    classdir = target[0]

    s = source[0].rentry().disambiguate()
    if isinstance(s, SCons.Node.FS.File):
        sourcedir = s.dir.rdir()
    elif isinstance(s, SCons.Node.FS.Dir):
        sourcedir = s.rdir()
    else:
        raise SCons.Errors.UserError("Java source must be File or Dir, not '%s'" % s.__class__)

    slist = []
    js = _my_normcase(java_suffix)
    for entry in source:
        entry = entry.rentry().disambiguate()
        if isinstance(entry, SCons.Node.FS.File):
            slist.append(entry)
        elif isinstance(entry, SCons.Node.FS.Dir):
            result = SCons.Util.OrderedDict()
            dirnode = entry.rdir()
            def find_java_files(arg, dirpath, filenames):
                java_files = sorted([n for n in filenames
                                       if _my_normcase(n).endswith(js)])
                mydir = dirnode.Dir(dirpath)
                java_paths = [mydir.File(f) for f in java_files]
                for jp in java_paths:
                     arg[jp] = True
            for dirpath, dirnames, filenames in os.walk(dirnode.get_abspath(),topdown=True):
               del dirnames[:]
               find_java_files(result, dirpath, filenames)
            entry.walk(find_java_files, result)

            slist.extend(list(result.keys()))
        else:
            raise SCons.Errors.UserError("Java source must be File or Dir, not '%s'" % entry.__class__)

    version = env.get('JAVAVERSION', '1.4')
    full_tlist = []
    for f in slist:
        tlist = []
        source_file_based = True
        pkg_dir = None
        if not f.is_derived():
            pkg_dir, classes = parse_java_file(f.rfile().get_abspath(), version)
            if classes:
                source_file_based = False
                if pkg_dir:
                    d = target[0].Dir(pkg_dir)
                    p = pkg_dir + os.sep
                else:
                    d = target[0]
                    p = ''
                for c in classes:
                    t = d.File(c + class_suffix)
                    t.attributes.java_classdir = classdir
                    t.attributes.java_sourcedir = sourcedir
                    t.attributes.java_classname = classname(p + c)
                    tlist.append(t)

        if source_file_based:
            base = f.name[:-len(java_suffix)]
            if pkg_dir:
                t = target[0].Dir(pkg_dir).File(base + class_suffix)
            else:
                t = target[0].File(base + class_suffix)
            t.attributes.java_classdir = classdir
            t.attributes.java_sourcedir = f.dir
            t.attributes.java_classname = classname(base)
            tlist.append(t)

        for t in tlist:
            t.set_specific_source([f])

        full_tlist.extend(tlist)

    return full_tlist, slist
