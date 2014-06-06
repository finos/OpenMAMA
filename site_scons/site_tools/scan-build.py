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

    if env.GetOption('clean'):
        return

    env['CC']    = os.environ['CC'] or 'ccc-analyzer'
    env['CXX']   = os.environ['CXX'] or 'c++-analyzer'
    env['CLANG'] = os.environ['CLANG']
    env['CLANG_CXX'] = os.environ['CLANG_CXX']

    env['ENV']['CC']    = env['CC']
    env['ENV']['CXX']   = env['CXX']
    env['ENV']['CLANG'] = env['CLANG']
    env['ENV']['CLANG_CXX'] = env['CLANG_CXX']

    for item in os.environ.items():
        if item[0].startswith('CCC_'):
            env[item[0]] = item[1]
            env['ENV'][item[0]] = item[1]

    env['CXXFLAGS'].append('-fcolor-diagnostics')
    env['CFLAGS'].append('-fcolor-diagnostics')

    env['ENV']['CXXFLAGS'] = env['CXXFLAGS']
    env['ENV']['CFLAGS']   = env['CFLAGS']

def exists(env):
    return env.Detect(compilers)
