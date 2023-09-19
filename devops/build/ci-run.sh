#!/bin/bash

. /etc/profile

# All errors fatal
set -e -x

export OPENMAMA_INSTALL_DIR=/opt/openmama
export SDKMAN_DIR=/usr/local/sdkman
BUILD_DIR=/build-ci-run
SOURCE_PATH_RELATIVE=$(dirname "$0")/../..
SOURCE_PATH_ABSOLUTE=$(cd "$SOURCE_PATH_RELATIVE" && pwd)

git config --global --add safe.directory $SOURCE_PATH_ABSOLUTE

# Build the project
if [ -d $BUILD_DIR ]
then
    rm -rf $BUILD_DIR
fi
/tmp/dotnet-install.sh -i ~/bin -c 6.0

export PATH=~/bin:$PATH

mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
     -DCMAKE_INSTALL_PREFIX=$OPENMAMA_INSTALL_DIR \
     -DWITH_JAVA=ON \
     -DWITH_CSHARP=ON \
     -DWITH_UNITTEST=ON \
     -DCMAKE_CXX_FLAGS="-Wno-error=strict-prototypes -Wno-deprecated-declarations" \
     -DCMAKE_C_FLAGS="-Wno-error=strict-prototypes -Wno-deprecated-declarations" \
     "$SOURCE_PATH_ABSOLUTE"
make -j install
export LD_LIBRARY_PATH=/opt/openmama/lib
ctest . --timeout 300 --output-on-failure
cd "$SOURCE_PATH_ABSOLUTE" > /dev/null

# Include the test data and grab profile configuration from there for packaging
if [ -d $OPENMAMA_INSTALL_DIR/data ]
then
    rm -rf $OPENMAMA_INSTALL_DIR/data
fi
mkdir -p $OPENMAMA_INSTALL_DIR/data
cd $OPENMAMA_INSTALL_DIR/data
curl -sL https://github.com/OpenMAMA/OpenMAMA-testdata/archive/master.tar.gz | tar xz --strip 1
cp profiles/profile.openmama .
rm -rf profiles
cd "$SOURCE_PATH_ABSOLUTE" > /dev/null

# Generate the package (deb / rpm / tarball).
# Globals
ARTIFACT_TYPE=${ARTIFACT_TYPE:-dev}

# Constants
RHEL=CentOS
UBUNTU=Ubuntu

VERSION=$(cat $BUILD_DIR/VERSION)
if [ -n "$GITHUB_REF" ]
then
    CURRENT_BRANCH=${GITHUB_REF##*/}
else
    CURRENT_BRANCH=$(git --git-dir="$SOURCE_PATH_ABSOLUTE/.git" rev-parse --abbrev-ref HEAD)
fi
echo "CURRENT_BRANCH='${CURRENT_BRANCH}'"
if echo "$VERSION" | grep -E "^[0-9.]*$" > /dev/null
then
    CLOUDSMITH_REPOSITORY=openmama
elif echo "$VERSION" | grep -E "^[0-9.]*-rc[0-9]*$" > /dev/null
then
    CLOUDSMITH_REPOSITORY=openmama-rc
elif [ "$CURRENT_BRANCH" = "next" ]
then
    CLOUDSMITH_REPOSITORY=openmama-experimental
else
    CLOUDSMITH_REPOSITORY=none
fi

if [ -z "$IS_DOCKER_BUILD" ]
then
    echo
    echo "------------------------------- DRAGON ALERT --------------------------------"
    echo "IS_DOCKER_BUILD environment not set. This should be set before launching this"
    echo "script as the root user IN AN ISOLATED DOCKER ENVIRONMENT. It is highly "
    echo "recommended not to run this script in any other environment. Obviously we"
    echo "cannot stop you... but if you try it, you're on your own..."
    echo "-----------------------------------------------------------------------------"
    echo
    exit $LINENO
fi

if [ -f /etc/redhat-release ]
then
    DISTRIB_RELEASE=$(cat /etc/redhat-release | tr " " "\n" | egrep "^[0-9]")
    DEPENDS_FLAGS="-d libuuid -d libevent -d ncurses -d apr -d apr-util -d java-11-openjdk"
    DISTRIB_ID=$RHEL
    PACKAGE_TYPE=rpm
    CLOUDSMITH_DISTRO_NAME=centos
    CLOUDSMITH_DISTRO_VERSION=${DISTRIB_RELEASE%%.*}
    if [ "$CLOUDSMITH_DISTRO_VERSION" = "7" ]
    then
        export LC_ALL=$(locale -a | tail -1)
        export LANG=$LC_ALL
    fi
fi

if [ -f /etc/lsb-release ]
then
    # Will set DISTRIB_ID and DISTRIB_RELEASE
    source /etc/lsb-release
    PACKAGE_TYPE=deb
    DEPENDS_FLAGS="-d uuid -d libevent-dev -d libzmq3-dev -d ncurses-dev -d libapr1-dev -d libaprutil1-dev -d openjdk-11-jdk"
    CLOUDSMITH_DISTRO_NAME=ubuntu
    CLOUDSMITH_DISTRO_VERSION=${DISTRIB_CODENAME}
fi

if [ "$DISTRIB_ID" == "$RHEL" ]
then
    if grep -q "Red Hat" /etc/redhat-release
    then
        DISTRIB_PACKAGE_QUALIFIER=el${DISTRIB_RELEASE%%.*}
    else
        DISTRIB_PACKAGE_QUALIFIER=centos${DISTRIB_RELEASE%%.*}
    fi
elif [ "$DISTRIB_ID" == "$UBUNTU" ]
then
    DISTRIB_PACKAGE_QUALIFIER=ubuntu${DISTRIB_RELEASE%%.*}
else
    echo "Unsupported distro [$DISTRIB_ID] found: $(cat /etc/*-release)" && exit $LINENO
fi

DIST_DIR="$SOURCE_PATH_ABSOLUTE/dist"
if [ ! -d "$DIST_DIR" ]
then
    mkdir "$DIST_DIR"
fi

PACKAGE_FILE="$DIST_DIR/openmama-$VERSION-1.$DISTRIB_PACKAGE_QUALIFIER.x86_64.$PACKAGE_TYPE"
fpm -s dir --force \
        -t $PACKAGE_TYPE \
        -m "openmama-users@lists.openmama.org" \
        --name openmama \
        --version $VERSION \
        --iteration 1 \
        --url "https://openmama.org" \
        --license LGPLv2 \
        $DEPENDS_FLAGS \
        -p "$PACKAGE_FILE" \
        --description "OpenMAMA high performance Market Data API" \
        $OPENMAMA_INSTALL_DIR/=/opt/openmama/

find "$DIST_DIR" -type f -name "*.$PACKAGE_TYPE"

echo "PACKAGE_UPLOAD_ENABLED = '$PACKAGE_UPLOAD_ENABLED'"
if [ "true" = "$PACKAGE_UPLOAD_ENABLED" ] && [ "$CLOUDSMITH_REPOSITORY" != "none" ] && [ "$CLOUDSMITH_API_KEY" != "" ]
then
    cloudsmith push $PACKAGE_TYPE "openmama/$CLOUDSMITH_REPOSITORY/$CLOUDSMITH_DISTRO_NAME/$CLOUDSMITH_DISTRO_VERSION" "${PACKAGE_FILE}"
fi
