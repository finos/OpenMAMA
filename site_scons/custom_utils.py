import platform,subprocess
import posixpath
import os,sys,re,string
import shutil,ConfigParser

import os
import sys
import platform

# workaround for http://bugs.python.org/issue7860
# platform.machine on 32b python returns 32 on 64b machine
def os_machine():
	"""Return type of machine."""
	if os.name == 'nt' and sys.version_info[:2] < (2,7):
		return os.environ.get("PROCESSOR_ARCHITEW6432", os.environ.get('PROCESSOR_ARCHITECTURE', ''))
	else:
		return platform.machine()

def os_bits(machine=os_machine()):
	"""Return bitness of operating system, or None if unknown."""
	machine2bits = {'AMD64': 64, 'x86_64': 64, 'i386': 32, 'x86': 32}
	return machine2bits.get(machine, None)

## Retrieve host information
# 
# Retrieves common host information required during the build process. Where
# necessary the information is normalised since values can differ between
# linux and windows e.g AMD64 vs. x86_64.
def getHostInfo():
    machine = os_machine()
    hostname = platform.node()
    pf = platform.platform()
    proc = platform.processor()
    system = platform.system()

    if machine == 'AMD64':
        machine = 'x86_64'

    host = {}
    host['arch'] = machine
    host['os'] = system
    host['hostname'] = hostname
    return host

def createDir( dir ):
    try:
        statinfo = os.stat(dir)
    except os.error,e:
        os.makedirs(dir)
