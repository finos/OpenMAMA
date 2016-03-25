#!/bin/bash

# We don't want anyone running this as root.
if [ "$EUID" -eq 0 ]; then
    echo "This script is not intended to be run as root. We don't sanitize input "
    echo "and delete entire directories on a regular basis. We REALLY REALLY REALLY"
    echo "recommend you run as an unprivilaged user."
    exit 1
fi

# Import a few helper functions...
. helper-functions.sh

print_help () {
cat <<EOF
usage: `basename $0` [-n|--next] [-v|--version <version>]
                       [-b|--build-number <build-number>]
                       [-B|--build-dir <build directory>]
                       [-d|--data-dir <data directory>]
                       [-s|--source-dir <source directory>]
                       [-r|--release-dir <release directory>]
                       [--env-only] [--clone-src-only] [--clone-data-only]
                       [--tar-source-only] [--rpm-only] [--mock-only]
                       [--package-only] [-h|-?|--help]

    -n|--next           Build the next branch.
    -V|--build-version  OpenMAMA Version to build.
    -b|--build-number   The build number (used for multiple builds of the same version).
    -B|--build-dir      The location of the build files. Defaults to {pwd}/build.
    -d|--data-dir       The location of the additional files used by the build. Defaults
                        to the current directory.
    -s|--source-dir     The location to which the source code is to be cloned. Defaults
                        to {pwd}/src.
    -r|--release-dir    The location into which the artifacts are to be moved once the build
                        has completed. Defaults to {pwd}/release.
    --env-only          Only prepare a new RPM build environment.
    --clone-src-only    Only clone down the OpenMAMA source. (Assumes the environment already exists.)
    --clone-data-only   Only clone down the sample data. (Assumes the environment already exists.)
    --tar-source-only   Only tar up the source code. (Assumes the source is already available.)
    --rpm-only          Only run the RPM build commands. (Assumes the source and env are available.)
    --mock-only         Only run the mock build commands. (Assumes the source RPM is available.)
    --package-only      Only package the build artifacts. (Assumes the RPMs, mock builds etc, are available.)
    -h|-?|--help        Displays this help text.
EOF
}

# Set the stages which must be performed:
BUILD_ENV=1
CLONE_SOURCE=1
CLONE_DATA=1
TAR_SOURCE=1
RPM_BUILD=1
MOCK_BUILD=1
PACKAGE_RELEASE=1

# Set the default values
VERSION="nightly"
BRANCH="next"
BUILD_NUMBER=1

# Setting the build directory to the current dir...
DATA_DIR=`pwd`/data
BUILD_DIR=`pwd`/build
SOURCE_DIR=`pwd`/src
RELEASE_DIR=`pwd`/release

# Set the return from each stage
RETURN_CODE=0

# Sort the command line options
while [[ $# > 0 ]]
do
key="$1"
shift

case $key in
    -n|--next)
    NEXT=YES
    VERSION="nightly"
    BRANCH="next"
    ;;
    -v|--version)
    VERSION="$1"
    BRANCH=OpenMAMA-${VERSION}
    shift
    ;;
    -b|--build-number)
    BUILD_NUMBER="$1"
    shift
    ;;
    -B|--build-dir)
    BUILD_DIR="$1"
    shift
    ;;
    -d|--data-dir)
    DATA_DIR="$1"
    shift
    ;;
    -s|--source-dir)
    SOURCE_DIR="$1"
    shift
    ;;
    -r|--release-dir)
    RELEASE_DIR="$1"
    shift
    ;;
    --env-only)
    BUILD_ENV=1
    CLONE_SOURCE=0
    CLONE_DATA=0
    TAR_SOURCE=0
    RPM_BUILD=0
    MOCK_BUILD=0
    PACKAGE_RELEASE=0
    ;;
    --clone-src-only)
    BUILD_ENV=0
    CLONE_SOURCE=1
    CLONE_DATA=0
    TAR_SOURCE=0
    RPM_BUILD=0
    MOCK_BUILD=0
    PACKAGE_RELEASE=0
    ;;
    --clone-data-only)
    BUILD_ENV=0
    CLONE_SOURCE=0
    CLONE_DATA=1
    TAR_SOURCE=0
    RPM_BUILD=0
    MOCK_BUILD=0
    PACKAGE_RELEASE=0
    ;;
    --tar-source-only)
    BUILD_ENV=0
    CLONE_SOURCE=0
    CLONE_DATA=0
    TAR_SOURCE=1
    RPM_BUILD=0
    MOCK_BUILD=0
    PACKAGE_RELEASE=0
    ;;
    --rpm-only)
    BUILD_ENV=0
    CLONE_SOURCE=0
    CLONE_DATA=0
    TAR_SOURCE=0
    RPM_BUILD=1
    MOCK_BUILD=0
    PACKAGE_RELEASE=0
    ;;
    --mock-only)
    BUILD_ENV=0
    CLONE_SOURCE=0
    CLONE_DATA=0
    TAR_SOURCE=0
    RPM_BUILD=0
    MOCK_BUILD=1
    PACKAGE_RELEASE=0
    ;;
    --package-only)
    BUILD_ENV=0
    CLONE_SOURCE=0
    CLONE_DATA=0
    TAR_SOURCE=0
    RPM_BUILD=0
    MOCK_BUILD=0
    PACKAGE_RELEASE=1
    ;;
    -h|-?|--help)
    print_help
    exit
    ;;
    *) # Unknown argument
    echo "Unknown argument. "
    print_help
    exit
    ;;
esac
done

echo BUILD DIR       = "${BUILD_DIR}"
echo SRC_DIR         = "${SOURCE_DIR}"
echo DATA_DIR        = "${DATA_DIR}"
echo VERSION         = "${VERSION}"
echo BRANCH          = "${BRANCH}"
echo BUILD NUMBER    = "${BUILD_NUMBER}"

if [ $BUILD_ENV -eq 1 ]; then
    step "Setup build environment. "

    if [ -d "${BUILD_DIR}" ]; then
        try rm -rf ${BUILD_DIR}
    fi

    # First step is to create a completely clean build structure:
    try mkdir -p ${BUILD_DIR}/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}

    # And tell rpmbuild where it's located...
    try echo '%_topdir '${BUILD_DIR} > ~/.rpmmacros
    try cp openmama.spec ${BUILD_DIR}/SPECS/

    next
    RETURN_CODE=$?
fi

if [ $CLONE_SOURCE -eq 1 ] && [ $RETURN_CODE -eq 0 ]; then
    step "Checking out clean source tree "

    if [ -d "${SOURCE_DIR}" ]; then
        try rm -rf ${SOURCE_DIR}
    fi

    try mkdir -p ${SOURCE_DIR}
    try cd ${SOURCE_DIR}

    # Read only clone, depth 1
    try git clone -q --depth=1 -b ${BRANCH} https://github.com/OpenMAMA/OpenMAMA.git

    # Remove the .git folder (for packaging)
    try rm -rf OpenMAMA/.git
    next
    RETURN_CODE=$?
fi

if [ $CLONE_DATA -eq 1 ] && [ $RETURN_CODE -eq 0 ]; then
    step "Checking out the OpenMAMA sample data."

    if [ -d "${DATA_DIR}" ]; then
        try rm -rf ${DATA_DIR}
    fi

    try git clone -q --depth=1 https://github.com/OpenMAMA/OpenMAMA-testdata.git
    try mv OpenMAMA-testdata ${DATA_DIR}
    try rm -rf ${DATA_DIR}/.git
    next
    RETURN_CODE=$?
fi

if [ $TAR_SOURCE -eq 1 ] && [ $RETURN_CODE -eq 0 ]; then
    step "Create the source tarball"
    try mv OpenMAMA openmama-${VERSION}
    try cp -r ${DATA_DIR} openmama-${VERSION}
    try tar -zcf openmama-${VERSION}.tgz openmama-${VERSION}
    try cp openmama-${VERSION}.tgz ${BUILD_DIR}/SOURCES/
    next
    RETURN_CODE=$?
fi

if [ $RPM_BUILD -eq 1 ] && [ $RETURN_CODE -eq 0 ]; then
    step "Perform the RPM Build"
    try cd ${BUILD_DIR}/SPECS
    try rpmbuild -ba --define "BUILD_VERSION "${VERSION} --define "BUILD_NUMBER "${BUILD_NUMBER} openmama.spec > ${BUILD_DIR}/rpmbuild.log 2>&1
    next
    RETURN_CODE=$?
fi

if [ $MOCK_BUILD -eq 1 ] && [ $RETURN_CODE -eq 0 ]; then
    step "Build multiplatform RPMs"
    try cd ${BUILD_DIR}/SRPMS
    try /usr/bin/mock -r epel-6-i386 --define 'BUILD_VERSION '${VERSION} --define 'BUILD_NUMBER '${BUILD_NUMBER} openmama-${VERSION}-${BUILD_NUMBER}.*.src.rpm > ${BUILD_DIR}/mock-el6-i386.log 2>&1
    try /usr/bin/mock -r epel-6-x86_64 --define 'BUILD_VERSION '${VERSION} --define 'BUILD_NUMBER '${BUILD_NUMBER} openmama-${VERSION}-${BUILD_NUMBER}.*.src.rpm > ${BUILD_DIR}/mock-el6-x64.log 2>&1
    try /usr/bin/mock -r epel-7-x86_64 --define 'BUILD_VERSION '${VERSION} --define 'BUILD_NUMBER '${BUILD_NUMBER} openmama-${VERSION}-${BUILD_NUMBER}.*.src.rpm > ${BUILD_DIR}/mock-el7-x64.log 2>&1
    try /usr/bin/mock -r fedora-21-x86_64 --define 'BUILD_VERSION '${VERSION} --define 'BUILD_NUMBER '${BUILD_NUMBER} openmama-${VERSION}-${BUILD_NUMBER}.*.src.rpm > ${BUILD_DIR}/mock-f21-x64.log 2>&1
    try /usr/bin/mock -r fedora-22-x86_64 --define 'BUILD_VERSION '${VERSION} --define 'BUILD_NUMBER '${BUILD_NUMBER} openmama-${VERSION}-${BUILD_NUMBER}.*.src.rpm > ${BUILD_DIR}/mock-f22-x64.log 2>&1
    next
    RETURN_CODE=$?
fi

if [ $PACKAGE_RELEASE -eq 1 ] && [ $RETURN_CODE -eq 0 ]; then
    step "Packaging release"

    if [ -d "${RELEASE_DIR}" ]; then
        try rm -rf ${RELEASE_DIR}
    fi

    try mkdir -p ${RELEASE_DIR}

    # Copy in and rename the source tarball
    try cp ${SOURCE_DIR}/openmama-${VERSION}.tgz ${RELEASE_DIR}/openmama-${VERSION}-src.tar.gz

    # Copy in the Source RPM
    try cp ${BUILD_DIR}/SRPMS/openmama-${VERSION}-${BUILD_NUMBER}.*.src.rpm ${RELEASE_DIR}

    # These paths are hard coded, since they should always remain there. We need
    # to update these periodically.
    try cp /var/lib/mock/epel-6-i386/result/openmama-${VERSION}-${BUILD_NUMBER}.el6.i686.rpm ${RELEASE_DIR}
    try cp /var/lib/mock/epel-6-x86_64/result/openmama-${VERSION}-${BUILD_NUMBER}.el6.x86_64.rpm ${RELEASE_DIR}
    try cp /var/lib/mock/epel-7-x86_64/result/openmama-${VERSION}-${BUILD_NUMBER}.el7.*.x86_64.rpm ${RELEASE_DIR}
    try cp /var/lib/mock/fedora-21-x86_64/result/openmama-${VERSION}-${BUILD_NUMBER}.fc21.x86_64.rpm ${RELEASE_DIR}
    try cp /var/lib/mock/fedora-22-x86_64/result/openmama-${VERSION}-${BUILD_NUMBER}.fc22.x86_64.rpm ${RELEASE_DIR}

    # Build and tar the binary release
    if [ -d ${BUILD_DIR}/binary ]; then
        try rm -rf ${BUILD_DIR}/binary
    fi

    BINARY_DIR=${BUILD_DIR}/binary/openmama-${VERSION}
    try mkdir -p ${BINARY_DIR}
    try mkdir ${BINARY_DIR}/data
    try mkdir ${BINARY_DIR}/config
    try cp -r ${BUILD_DIR}/BUILD/openmama-${VERSION}/data/* ${BINARY_DIR}/data/
    try cp -r ${BUILD_DIR}/BUILD/openmama-${VERSION}/openmama_install*/* ${BINARY_DIR}/
    try cp ${BUILD_DIR}/BUILD/openmama-${VERSION}/data/profiles/profile.openmama ${BINARY_DIR}/config/
    try cp ${BUILD_DIR}/BUILD/openmama-${VERSION}/README.md ${BINARY_DIR}/README
    try cp ${BUILD_DIR}/BUILD/openmama-${VERSION}/COPYING ${BINARY_DIR}/
    try cp ${BUILD_DIR}/BUILD/openmama-${VERSION}/mama/c_cpp/src/examples/mama.properties ${BINARY_DIR}/config/

    try cd ${BUILD_DIR}/binary/
    try tar -zcf ${RELEASE_DIR}/openmama-${VERSION}-x86_64.tar.gz openmama-${VERSION}

    next
    RETURN_CODE=$?
fi

exit $RETURN_CODE
