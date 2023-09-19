#!/bin/bash

# All errors fatal
set -x

. /etc/profile

set -e

unset PREFIX

# Install dotnet
wget --no-check-certificate -q -O /tmp/dotnet-install.sh https://dot.net/v1/dotnet-install.sh
chmod a+x /tmp/dotnet-install.sh
/tmp/dotnet-install.sh -i ~/bin -c 6.0

brew install googletest apr apr-util ossp-uuid qpid-proton

# Build the project
export CMAKE_SOURCE_DIR=$(pwd)
export CMAKE_BUILD_DIR=$(pwd)/build
test -d $CMAKE_BUILD_DIR && rm -rf $CMAKE_BUILD_DIR || true
mkdir -p $CMAKE_BUILD_DIR
cd $CMAKE_BUILD_DIR
export CMAKE_INSTALL_DIR=$(pwd)/install
export DYLD_LIBRARY_PATH=$CMAKE_INSTALL_DIR/lib
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
     -DCMAKE_BUILD_TYPE=RelWithDebInfo \
     -DCMAKE_INSTALL_PREFIX=$CMAKE_INSTALL_DIR \
     -DWITH_JAVA=ON \
     -DWITH_CSHARP=ON \
     -DWITH_UNITTEST=ON \
     -DCMAKE_CXX_STANDARD=17 \
     ..
make -j install
ctest . -E java_unittests --timeout 240 --output-on-failure

# Return to parent directory
cd - > /dev/null

# Include the test data and grab profile configuration from there for packaging
test -d $CMAKE_INSTALL_DIR/data && rm -rf $CMAKE_INSTALL_DIR/data || true
mkdir -p $CMAKE_INSTALL_DIR/data
cd $CMAKE_INSTALL_DIR/data
curl -sL https://github.com/OpenMAMA/OpenMAMA-testdata/archive/master.tar.gz | tar xz --strip 1
cp profiles/profile.openmama .
rm -rf profiles
cd - > /dev/null

# Generate the package (deb / rpm / tarball).
VERSION=$(cat $CMAKE_BUILD_DIR/VERSION)
export OPENMAMA_RELEASE_DIR=$CMAKE_SOURCE_DIR/openmama-$VERSION.osx.$(sw_vers -productVersion).$(uname -m)
mv $CMAKE_INSTALL_DIR $OPENMAMA_RELEASE_DIR
7z a $OPENMAMA_RELEASE_DIR.zip "$OPENMAMA_RELEASE_DIR" || goto error