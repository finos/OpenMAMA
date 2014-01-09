"""SCons.Tool.clang++

Tool-specific initialization for clang++.

"""
import os
import subprocess

import SCons.Tool
import SCons.Util

sconsmod = __import__ ('SCons.Tool.g++', globals(), locals(), [])
sconstool = getattr (sconsmod, 'Tool')
gplusplus = getattr (sconstool, 'g++')

compilers = ['clang++']

def generate(env):
    """Add Builders and construction variables for clang++ to an Environment."""
    gplusplus.generate(env)
    env['CXX']        = env.Detect(compilers) or 'clang++'
    env['CLANG_CXX']  = env.Detect(compilers) or 'clang++'
    env['CXXFLAGS'].append('-fcolor-diagnostics')

def exists(env):
    return env.Detect(compilers)
