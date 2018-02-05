import os
import subprocess
import sys
import signal

import globals

def start_app(app,tport,args=None,stdout=None,stderr=None):
    app_path = os.path.join(globals.TOOLS_PATH,app)
    if not os.path.isfile(app_path):
        my_print(str(app_path) + "file does not exist")
        exit(1)
    ret = None
    if globals.TRANSPORTPUB is None:
        arglist = [app_path,"-m",globals.MIDDLEWARE]
    else:
        if tport=='pub':
            arglist = [app_path,"-m",globals.MIDDLEWARE,"-tport",globals.TRANSPORTPUB]
        elif tport=='sub':
            arglist = [app_path,"-m",globals.MIDDLEWARE,"-tport",globals.TRANSPORTSUB]

    if args is None:
        ret = subprocess.Popen(arglist,stdout=stdout,stderr=stderr)
    else:
        arglist.extend(args)
        ret = subprocess.Popen(arglist,stdout=stdout,stderr=stderr)
    
    return ret

def start_javaapp(app,tport,args=None,stdout=None,stderr=None):
    ret = None
    if globals.TRANSPORTPUB is None:
        arglist = ["java",app,"-m",globals.MIDDLEWARE]
    else:
        if tport=='pub':
            arglist = ['java',app,"-m",globals.MIDDLEWARE,"-tport",globals.TRANSPORTPUB]
        elif tport=='sub':
            arglist = ['java',app,"-m",globals.MIDDLEWARE,"-tport",globals.TRANSPORTSUB]
    
    if args is None:
        ret = subprocess.Popen(arglist,stdout=stdout,stderr=stderr)
    else:
        arglist.extend(args)
        ret = subprocess.Popen(arglist,stdout=stdout,stderr=stderr)
    return ret
def call_wait4text(text,file,args=None):
    ret = 1
    arglist = [sys.executable,"wait4text.py",text,file]
    if args is None:
        arglist.extend(["-t",str(globals.TIMEOUT)])
        if globals.VERBOSE is False:
            arglist.extend(["-q"])
        ret = subprocess.call(arglist)
    else:
        arglist.extend(args)
        ret = subprocess.call(arglist)
          
    return ret
def call_comparelog(expected,actual,verbose=True):
    ret = 1
    if globals.VERBOSE is False:
        ret = subprocess.call([sys.executable,"comparelog.py",expected,actual,"-q"])
    else:
        ret = subprocess.call([sys.executable,"comparelog.py",expected,actual])
    return ret

def check_env():
    '''check that the LD_LIBRARY_PATH is set'''
    if not (2,3) < sys.version_info < (3,3):
        sys.exit("Please use python version between 2.3 and 2.7")

    try:
        ld_library_path = os.environ["LD_LIBRARY_PATH"]
    except:
        my_print("LD_LIBRARY_PATH is not set")
        sys.exit(1)
    try:
        globals.TOOLS_PATH = os.environ["TOOLS_PATH"]
    except KeyError:
        my_print("TOOLS_PATH set to " + str(globals.TOOLS_PATH))

def kill_process(pid):
    if sys.platform == 'win32':
        import ctypes
        PROCESS_TERMINATE = 1
        handle = ctypes.windll.kernel32.OpenProcess(PROCESS_TERMINATE,false,pid)
        ctypes.windll.kernel32.TerminateProcess(handle,-1)
        ctypes.windll.kernel32.CloseHandle(handle)
    else:
        os.kill(pid, signal.SIGKILL)

def my_print(text,verbose=True):
    if verbose is True:
        sys.stdout.write(text)
        sys.stdout.write("\n")
