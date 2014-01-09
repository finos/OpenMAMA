"""SCons.Tool.clang

Tool-specific initialization for clang.

"""
import re
import subprocess
import os

import SCons.Tool
import SCons.Util

sconsmod = __import__ ('SCons.Tool.g++', globals(), locals(), [])
sconstool = getattr (sconsmod, 'Tool')
gplusplus = getattr (sconstool, 'g++')

compilers = ['clang', 'clang++', 'ccc-analyzer', 'c++-analyzer']

def generate(env):
    """Setup GCC and G++ Env first"""
    SCons.Tool.gcc.generate(env)
    gplusplus.generate(env)

    """Add Builders and construction variables for gcc to an Environment."""
    env['CC']    = os.environ['CC'] or 'ccc-analyzer'
    env['CXX']   = os.environ['CXX'] or 'c++-analyzer'
    env['CLANG'] = os.environ['CLANG']
    env['CLANG_CXX'] = os.environ['CLANG_CXX']

    for item in os.environ.items():
        if item[0].startswith('CCC_'):
            print item
            env[item[0]] = item[1]

    env['CXXFLAGS'].append('-fcolor-diagnostics')
    env['CFLAGS'].append('-fcolor-diagnostics')

def exists(env):
    return env.Detect(compilers)
