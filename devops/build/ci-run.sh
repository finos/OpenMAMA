#!/bin/bash

# All errors fatal
set -x

. /etc/profile

set -e

export OPENMAMA_INSTALL_DIR=/opt/openmama
unset PREFIX
unset VERSION_FILE

# Build the project
if [ -d build ]
then
    rm -rf build
fi

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
     -DBIN_GRADLE=$SDKMAN_DIR/candidates/gradle/current/bin/gradle \
     -DCMAKE_BUILD_TYPE=RelWithDebInfo \
     -DCMAKE_INSTALL_PREFIX=$OPENMAMA_INSTALL_DIR \
     -DWITH_JAVA=ON \
     -DWITH_CSHARP=OFF \
     -DWITH_UNITTEST=ON \
     -DCMAKE_CXX_FLAGS="-Werror -Wno-error=strict-prototypes -Wno-deprecated-declarations" \
     -DCMAKE_C_FLAGS="-Werror -Wno-error=strict-prototypes -Wno-deprecated-declarations" \
     ..
make -j install
export LD_LIBRARY_PATH=/opt/openmama/lib
ctest .
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
./devops/build/build-package.sh
