#!/bin/bash

# All errors fatal
set -x

. /etc/profile

set -e

unset PREFIX
unset VERSION_FILE

brew install googletest apr apr-util ossp-uuid qpid-proton

APR_ROOT=$(find /usr/local/Cellar/apr -type d -name libexec)

# Build the project
test -d build && rm -rf build || true
mkdir -p build
cd build
export OPENMAMA_INSTALL_DIR=`pwd`/install
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
     -DCMAKE_BUILD_TYPE=RelWithDebInfo \
     -DCMAKE_INSTALL_PREFIX=$OPENMAMA_INSTALL_DIR \
     -DWITH_JAVA=ON \
     -DWITH_CSHARP=OFF \
     -DAPR_ROOT=$APR_ROOT \
     -DWITH_UNITTEST=ON \
     -DCMAKE_CXX_STANDARD=17 \
     ..
make install
cd - > /dev/null

# Perform unit tests
ctest .

# Include the test data and grab profile configuration from there for packaging
test -d $OPENMAMA_INSTALL_DIR/data && rm -rf $OPENMAMA_INSTALL_DIR/data || true
mkdir -p $OPENMAMA_INSTALL_DIR/data
cd $OPENMAMA_INSTALL_DIR/data
curl -sL https://github.com/OpenMAMA/OpenMAMA-testdata/archive/master.tar.gz | tar xz --strip 1
cp profiles/profile.openmama .
rm -rf profiles
cd - > /dev/null

# Generate the package (deb / rpm / tarball).
