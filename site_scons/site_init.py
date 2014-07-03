import platform,subprocess
import posixpath
import os,sys,re,string
import shutil,ConfigParser
import ctypes
import SCons.Action
import SCons.Builder
import SCons.Util
from SCons.Script import *
from SCons.Node.FS import _my_normcase
from SCons.Tool.JavaCommon import parse_java_file

from version_helper import *
from jni_tools import *

#TODO - find a way to allow these to work for open and ent
# it was previously in a try/except
from logger import Logger
try:
    from enterprise.linux import Linux
    from enterprise.windows import Windows
    from enterprise.command_line import *
    from enterprise.release_note_helper import *
    from enterprise.log_helper import *
    from enterprise.test import *
except ImportError:
    from community.linux import Linux
    from community.windows import Windows
    from community.darwin import Darwin
    from community.command_line import *
    from community.log_helper import *
