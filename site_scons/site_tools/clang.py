"""SCons.Tool.clang

Tool-specific initialization for clang.

"""

from SCons.Tool import gcc
import re
import subprocess
import os

import SCons.Util

compilers = ['clang']

def generate(env):
    """Add Builders and construction variables for clang to an Environment."""
    gcc.generate(env)
    env['CC'] = env.Detect(compilers) or 'clang'
    env['CLANG'] = env.Detect(compilers) or 'clang'
    env['CFLAGS'].append('-fcolor-diagnostics')

def exists(env):
    return env.Detect(compilers)
