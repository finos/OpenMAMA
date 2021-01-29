#!/usr/bin/env python

import subprocess
import os
from dirsync import sync

openmama_src_root = os.path.join(os.path.dirname(os.path.realpath(__file__)), "..")

def run_command(args, relpath, shell=True):
    print ("Starting to run command {}".format(" ".join(args)))
    p = subprocess.Popen(args,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT,
                         shell=shell,
                         cwd=os.path.join(openmama_src_root, relpath))

    for line in iter(p.stdout.readline, b''):
        print(line.rstrip())

    # Wait for execution to complete
    p.wait()
    if p.returncode != 0:
        raise subprocess.CalledProcessError(p.returncode, subprocess.list2cmdline(args))
    print ("Finished running command {}".format(" ".join(args)))

# Will output to mama/c_cpp/doc/c/html
run_command(args=["doxygen", "doxyconfig-c.in"], relpath=os.path.join("mama", "c_cpp"))
sync("{}/mama/c_cpp/doc/c/html".format(openmama_src_root), "./reference/mama/c", "sync", create=True)
# Will output to mama/c_cpp/doc/cpp/html
run_command(args=["doxygen", "doxyconfig-cpp.in"], relpath=os.path.join("mama", "c_cpp"))
sync("{}/mama/c_cpp/doc/cpp/html".format(openmama_src_root), "./reference/mama/cpp", "sync", create=True)
# Will output to mamda/c_cpp/doc/cpp/html
run_command(args=["doxygen", "doxyconfig-cpp.in"], relpath=os.path.join("mamda", "c_cpp"))
sync("{}/mamda/c_cpp/doc/cpp/html".format(openmama_src_root), "./reference/mamda/cpp", "sync", create=True)

## Below are the new doxygen generated output, hence the different conventions. We will
## merge these eventually

# Will output to mama/jni/generated_docs/html
run_command(args=["doxygen", "doxyconfig-java.in"], relpath=os.path.join("mama", "jni"))
sync("{}/mama/jni/generated_docs/html".format(openmama_src_root), "./reference/mama/java", "sync", create=True)
# Will output to mamda/java/generated_docs/html
run_command(args=["doxygen", "doxyconfig-java.in"], relpath=os.path.join("mamda", "java"))
sync("{}/mamda/java/generated_docs/html".format(openmama_src_root), "./reference/mamda/java", "sync", create=True)
# Will output to mama/dotnet/generated_docs/html
run_command(args=["doxygen", "doxyconfig-cs.in"], relpath=os.path.join("mama", "dotnet"))
sync("{}/mama/dotnet/generated_docs/html".format(openmama_src_root), "./reference/mama/cs", "sync", create=True)
# Will output to mamda/dotnet/generated_docs/html
run_command(args=["doxygen", "doxyconfig-cs.in"], relpath=os.path.join("mamda", "dotnet"))
sync("{}/mamda/dotnet/generated_docs/html".format(openmama_src_root), "./reference/mamda/cs", "sync", create=True)
