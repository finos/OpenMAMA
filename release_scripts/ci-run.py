#!/usr/bin/env python

import subprocess, os

def run_command(args, fatal_error=True, env=None, shell=True):
    # Run the command
    p = subprocess.Popen(args,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT,
                         env=env,
                         shell=shell)

    for line in iter(p.stdout.readline, b''):
        print(line.rstrip())

    # Wait for execution to complete
    p.wait()
    if p.returncode != 0:
        # 1 is a test failure, anything else may well be a crash, so make it fatal
        if fatal_error or p.returncode != 1:
            raise subprocess.CalledProcessError(p.returncode, subprocess.list2cmdline(args))
        else:
            print "WARNING: %s returned error '%s'" % (subprocess.list2cmdline(args), p.returncode)

env_var = os.environ.copy()
env_var["PATH"] = env_var["PATH"]

# Set up defaults if not specified
if "PAYLOAD" not in env_var:
    env_var["PAYLOAD"] = 'qpidmsg' 

if "MW" not in env_var:
    env_var["MW"] = 'qpid'

if "MWVER" not in env_var:
    env_var["MWVER"] = 'default'

if "MSVSVER" not in env_var:
    env_var["MSVSVER"] = 'VS2015'

# Set up derived values
if os.name is 'nt':
    dep_base = os.path.join("C:\\", "Deps", env_var["MSVSVER"])
    shell = True
    #product = 'mamdaall'
    product = 'mamda'
else:
    dep_base = '/opt'
    shell = False
    #product = 'mamdajni'
    product = 'mamda'
gtest_home = os.path.join(dep_base, 'gtest', 'default')
middleware = env_var["MW"].lower()
payload = env_var["PAYLOAD"].lower()
middleware_home = os.path.join(dep_base, middleware, env_var["MWVER"])
libevent_home = os.path.join(dep_base, 'libevent', 'default')
nunit_home = os.path.join(dep_base, 'nunit', 'default')
payload_id = payload[0].upper()

scons_cmd = ["scons",
             "middleware=%s" % middleware,
             "gtest_home=%s" % gtest_home,
             "with_unittest=yes",
             "%s_home=%s" % (middleware, middleware_home),
             "entitlements=noop",
             "product=%s" % product
            ]

if os.name is 'nt':
    scons_cmd.append("libevent_home=%s" % libevent_home)
    scons_cmd.append("nunit_home=%s" % nunit_home)
    scons_cmd.append("dotnet_version=4.0")
    scons_cmd.append("blddir=objdir_64")
else:
    scons_cmd.append("java_home=/usr/java/default")
    scons_cmd.append("junit_home=/usr/share/java")


# Fire off the build
run_command(args=scons_cmd, env=env_var, shell=shell)

# Installation directory just created
install_dir = [d for d in os.listdir('.') if os.path.isdir(d) and d.startswith("openmama_install")][0]

# Set up environment for unit tests
env_var["LD_LIBRARY_PATH"] = "%s:%s:%s:%s:%s:%s:%s:%s:%s" % (
                                 os.path.join(os.getcwd(), install_dir, 'lib'),
                                 os.path.join(middleware_home, 'lib'),
                                 os.path.join(middleware_home, 'lib64'),
                                 os.path.join(libevent_home, 'lib'),
                                 os.path.join(libevent_home, 'lib64'),
                                 '/usr/local/lib',
                                 '/usr/local/lib64',
                                 '/usr/local/lib64',
                                 gtest_home)

env_var["WOMBAT_PATH"] = os.path.join(dep_base, 'openmama', 'config')

found_tests = []
for root, dirs, files in os.walk(install_dir):
    for file in files:
        if file.startswith("UnitTest") and not file.endswith(".pdb") and not file in found_tests:
            found_tests.append(file)
            if file.endswith(".exe"):
                # Windows test
                run_command(args=[os.path.join(root, file),
                                  "-m",
                                  middleware,
                                  "-p",
                                  payload,
                                  "-i",
                                  payload_id,
                                  "--gtest_output=xml:%s.xml" % file[0:-4]
                                 ],
                            fatal_error=False,
                            shell=shell)
            else:
                # Linux test
                run_command(args=["valgrind",
                                  "--tool=memcheck",
                                  "--leak-check=full",
                                  "--show-reachable=no",
                                  "--undef-value-errors=no",
                                  "--track-origins=no",
                                  "--xml=yes",
                                  "--xml-file=%s.result" % file,
                                  os.path.join('.', root, file),
                                  "-m",
                                  middleware,
                                  "-p",
                                  payload,
                                  "-i",
                                  payload_id,
                                  "--gtest_output=xml:%s.xml" % file
                                 ],
                            fatal_error=False,
                            shell=shell,
                            env=env_var)
