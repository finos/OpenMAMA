#!/usr/bin/env python

import subprocess
import os
import urllib

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
            print("WARNING: %s returned error '%s'" % (subprocess.list2cmdline(args), p.returncode))

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

# Go download junit
urllib.urlretrieve ("http://search.maven.org/remotecontent?filepath=junit/junit/4.12/junit-4.12.jar", "junit.jar")
urllib.urlretrieve ("http://search.maven.org/remotecontent?filepath=org/hamcrest/hamcrest-core/1.3/hamcrest-core-1.3.jar", "hamcrest-core.jar")
junit_home = os.getcwd()

# Set up derived values
if os.name is 'nt':
    dep_base = os.path.join("C:\\", "Deps", env_var["MSVSVER"])
    shell = True
    product = 'mamdaall'
    env_var["CCFLAGS"] = '/WX'
    env_var["CXXFLAGS"] = '/WX'
else:
    dep_base = '/opt'
    shell = False
    # Have to disable error for strict-prototypes otherwise the
    # CheckLib generated code on scons will fail. Bring on cmake :(
    env_var["CCFLAGS"] = '-Werror -Wno-error=strict-prototypes'
    env_var["CXXFLAGS"] = '-Werror -Wno-error=strict-prototypes'
    product = 'mamdajni'

middleware = env_var["MW"].lower()
payload = env_var["PAYLOAD"].lower()
payload_id = payload[0].upper()

scons_cmd = ["scons",
             "with_unittest=yes",
             "product=%s" % product,
             "junit_home=%s" % junit_home
            ]

if os.name != "nt" and "JAVA_HOME" not in env_var:
    scons_cmd.append("java_home=/usr/java/default")

if "JAVA_HOME" in env_var:
    scons_cmd.append("java_home=%s" % env_var["JAVA_HOME"])
    env_var["PATH"] = os.path.join(env_var["JAVA_HOME"], 'bin') + os.pathsep + env_var["PATH"]
    java_bin = os.path.join(env_var["JAVA_HOME"], 'bin', 'java')
    print("PATH={}".format(env_var["PATH"]))
else:
    print("UNTAINTED PATH={}".format(env_var["PATH"]))
    java_bin = "java"

run_command(args=["java",
                  "-version"],
            fatal_error=True,
            shell=shell,
            env=env_var)

run_command(args=[java_bin,
                  "-version"],
            fatal_error=True,
            shell=shell,
            env=env_var)

if "OPENMAMA_INSTALL_DIR" not in env_var:
    # Fire off the build
    run_command(args=scons_cmd, env=env_var, shell=shell)

    # Installation directory just created
    install_dir = [d for d in os.listdir('.') if os.path.isdir(d) and d.startswith("openmama_install")][0]
else:
    install_dir = env_var["OPENMAMA_INSTALL_DIR"]

# Set up environment for unit tests
if os.name != 'nt':
    env_var["LD_LIBRARY_PATH"] = "%s:%s:%s" % (
                                     os.path.join(os.getcwd(), install_dir, 'lib'),
                                     '/usr/local/lib',
                                     '/usr/local/lib64')
    mama_jni_jar = os.path.join(os.getcwd(), install_dir, 'lib', 'mamajni.jar')
else:
    env_var["PATH"] = os.path.join(os.getcwd(), install_dir, 'bin', 'dynamic') + os.pathsep + os.path.join(os.getcwd(), install_dir, 'bin') + os.pathsep + env_var["PATH"]
    mama_jni_jar = os.path.join(os.getcwd(), install_dir, 'lib', 'dynamic', 'mamajni.jar')
    if not os.path.exists(mama_jni_jar):
        mama_jni_jar = os.path.join(os.getcwd(), install_dir, 'lib', 'mamajni.jar')
    mama_nunit_dll = os.path.join(os.getcwd(), install_dir, 'bin', 'dynamic', 'DOTNET_UNITTESTS.dll')
    if not os.path.exists(mama_nunit_dll):
        mama_nunit_dll = os.path.join(os.getcwd(), install_dir, 'bin', 'DOTNET_UNITTESTS.dll')

env_var["WOMBAT_PATH"] = os.path.join(os.getcwd(), 'mama', 'c_cpp', 'src', 'examples') + os.pathsep + os.path.join(os.getcwd(), 'mama', 'c_cpp', 'src', 'gunittest', 'c')
mama_java_test_classes_dir = os.path.join(os.getcwd(), 'build', 'mama', 'jni', 'src', 'mama_java_build', 'classes', 'java', 'test')
if not os.path.exists(mama_java_test_classes_dir):
    mama_java_test_classes_dir = os.path.join(os.getcwd(), 'build', 'mama', 'jni', 'src', 'mama_java_build', 'classes', 'test')

if "JOB_NAME" in env_var:
    test_failure_fatal = False
else:
    test_failure_fatal = True

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
                            fatal_error=test_failure_fatal,
                            shell=shell,
                            env=env_var)
            else:
                # Linux test
                run_command(args=["valgrind",
                                  "--tool=memcheck",
                                  "--leak-check=full",
                                  "--show-reachable=no",
                                  "--undef-value-errors=no",
                                  "--track-origins=no",
                                  "--suppressions=release_scripts/intentionalunittestleaks.supp",
                                  "--xml=yes",
                                  #"--error-exitcode=%d" % int(test_failure_fatal),
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
                            fatal_error=test_failure_fatal,
                            shell=shell,
                            env=env_var)

# These test runners don't generate proper xml with this runner so just
# mark as error code (will look like a build failure but at least CI
# will know it's broken)
run_command(args=["java",
                  "-cp",
                  mama_jni_jar + os.pathsep + mama_java_test_classes_dir + os.pathsep + os.path.join(junit_home, "junit.jar") + os.pathsep + os.path.join(junit_home, "hamcrest-core.jar"),
                  "com.wombat.mama.junittests.Main",
                  "-m",
                  middleware
                 ],
            fatal_error=True,
            shell=shell,
            env=env_var)

nunit_console = "C:\\Program Files (x86)\\NUnit 2.6.4\\bin\\nunit-console.exe"
if not os.path.exists(nunit_console):
    nunit_console = "nunit3-console"

if os.name == "nt":
    env_var["middlewareName"] = middleware
    env_var["transportName"] = "pub"
    run_command(args=[
                  nunit_console,
                  mama_nunit_dll
                ],
            fatal_error=True,
            shell=shell,
            env=env_var)
