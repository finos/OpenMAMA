#!/bin/bash

# All errors fatal
set -x

. /etc/profile

set -e

unset PREFIX
unset VERSION_FILE

# Install dotnet
wget --no-check-certificate -q -O /tmp/dotnet-install.sh https://dot.net/v1/dotnet-install.sh
chmod a+x /tmp/dotnet-install.sh
/tmp/dotnet-install.sh -i ~/bin -c 3.1
/tmp/dotnet-install.sh -i ~/bin

brew install googletest apr apr-util ossp-uuid qpid-proton

APR_ROOT=$(find /usr/local/Cellar/apr -type d -name libexec)
APRUTIL_ROOT=$(find /usr/local/Cellar/apr-util -type d -maxdepth 1 -mindepth 1)

# Build the project
test -d build && rm -rf build || true
mkdir -p build
cd build
export OPENMAMA_INSTALL_DIR=`pwd`/install
export DYLD_LIBRARY_PATH=$OPENMAMA_INSTALL_DIR/lib
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
     -DCMAKE_BUILD_TYPE=RelWithDebInfo \
     -DCMAKE_INSTALL_PREFIX=$OPENMAMA_INSTALL_DIR \
     -DWITH_JAVA=ON \
     -DWITH_CSHARP=ON \
     -DAPR_ROOT=$APR_ROOT \
     -DAPRUTIL_ROOT=$APRUTIL_ROOT \
     -DWITH_UNITTEST=ON \
     -DCMAKE_CXX_STANDARD=17 \
     ..
make -j install
ctest . -E java_unittests --timeout 120 --output-on-failure

# Return to parent directory
cd - > /dev/null

# Include the test data and grab profile configuration from there for packaging
test -d $OPENMAMA_INSTALL_DIR/data && rm -rf $OPENMAMA_INSTALL_DIR/data || true
mkdir -p $OPENMAMA_INSTALL_DIR/data
cd $OPENMAMA_INSTALL_DIR/data
curl -sL https://github.com/OpenMAMA/OpenMAMA-testdata/archive/master.tar.gz | tar xz --strip 1
cp profiles/profile.openmama .
rm -rf profiles
cd - > /dev/null

# Generate the package (deb / rpm / tarball).
