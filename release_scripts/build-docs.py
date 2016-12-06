#!/usr/bin/env python

import subprocess, os

def run_command(args, relpath, shell=True):
    print ("Starting to run command {}".format(" ".join(args)))
    p = subprocess.Popen(args,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT,
                         shell=shell,
                         cwd=os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", relpath))

    for line in iter(p.stdout.readline, b''):
        print(line.rstrip())

    # Wait for execution to complete
    p.wait()
    if p.returncode != 0:
        raise subprocess.CalledProcessError(p.returncode, subprocess.list2cmdline(args))
    print ("Finished running command {}".format(" ".join(args)))

# Will output to mama/c_cpp/doc/c
run_command(args=["doxygen", "doxyconfig-c.in"], relpath=os.path.join("mama", "c_cpp"))
# Will output to mama/c_cpp/doc/cpp
run_command(args=["doxygen", "doxyconfig-cpp.in"], relpath=os.path.join("mama", "c_cpp"))
# Will output to mamda/c_cpp/doc/cpp
run_command(args=["doxygen", "doxyconfig-cpp.in"], relpath=os.path.join("mamda", "c_cpp"))

## Below are the new doxygen generated output, hence the different conventions. We will
## merge these eventually

# Will output to mama/jni/generated_docs
run_command(args=["doxygen", "doxyconfig-java.in"], relpath=os.path.join("mama", "jni"))
# Will output to mamda/java/generated_docs
run_command(args=["doxygen", "doxyconfig-java.in"], relpath=os.path.join("mamda", "java"))
# Will output to mama/dotnet/generated_docs
run_command(args=["doxygen", "doxyconfig-cs.in"], relpath=os.path.join("mama", "dotnet"))
# Will output to mamda/dotnet/generated_docs
run_command(args=["doxygen", "doxyconfig-cs.in"], relpath=os.path.join("mamda", "dotnet"))