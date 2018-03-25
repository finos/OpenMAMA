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
                       [--src-rpm-only] [--mock-only]
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
    --src-rpm-only      Only run the Source RPM build commands. (Assumes the source and env are available.)
    --mock-only         Only run the mock build commands. (Assumes the source RPM is available.)
    --package-only      Only package the build artifacts. (Assumes the RPMs, mock builds etc, are available.)
    -h|-?|--help        Displays this help text.
EOF
}

# Set the stages which must be performed:
BUILD_ENV=1
CLONE_SOURCE=1
CLONE_DATA=1
SRC_RPM_BUILD=1
MOCK_BUILD=1
PACKAGE_RELEASE=1

# Set the default values
VERSION="snapshot"
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
    VERSION="snapshot"
    BRANCH="next"
    ;;
    -v|--version)
    BRANCH=OpenMAMA-$1
    VERSION="${1//-/}"
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
    SRC_RPM_BUILD=0
    MOCK_BUILD=0
    PACKAGE_RELEASE=0
    ;;
    --clone-src-only)
    BUILD_ENV=0
    CLONE_SOURCE=1
    CLONE_DATA=0
    SRC_RPM_BUILD=0
    MOCK_BUILD=0
    PACKAGE_RELEASE=0
    ;;
    --clone-data-only)
    BUILD_ENV=0
    CLONE_SOURCE=0
    CLONE_DATA=1
    SRC_RPM_BUILD=0
    MOCK_BUILD=0
    PACKAGE_RELEASE=0
    ;;
    --src-rpm-only)
    BUILD_ENV=0
    CLONE_SOURCE=0
    CLONE_DATA=0
    SRC_RPM_BUILD=1
    MOCK_BUILD=0
    PACKAGE_RELEASE=0
    ;;
    --mock-only)
    BUILD_ENV=0
    CLONE_SOURCE=0
    CLONE_DATA=0
    SRC_RPM_BUILD=0
    MOCK_BUILD=1
    PACKAGE_RELEASE=0
    ;;
    --package-only)
    BUILD_ENV=0
    CLONE_SOURCE=0
    CLONE_DATA=0
    SRC_RPM_BUILD=0
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

# Mock directory to pull the tarball release out of
MOCK_ROOT=/var/lib/mock
MOCK_CFG_RELEASE=epel-6-x86_64
# Extracts all mock configs
ALL_MOCK_CFG=$(echo /etc/mock/epel-{6,7}*86* /etc/mock/fedora-[0-9]*86_64* | sed  -e "s#[^ ]*/##g" -e "s#.cfg##g")
LOG_STEPS=$BUILD_DIR/steps.log

# If this is a snapshot build and no build number has been specified
if [ "$VERSION" = "snapshot" ] && [ $BUILD_NUMBER -eq 1 ]; then
    BUILD_NUMBER=$(date +%Y%m%d%H%M%S)
fi

echo BUILD DIR        = "${BUILD_DIR}"
echo SRC_DIR          = "${SOURCE_DIR}"
echo DATA_DIR         = "${DATA_DIR}"
echo VERSION          = "${VERSION}"
echo BRANCH           = "${BRANCH}"
echo BUILD NUMBER     = "${BUILD_NUMBER}"
echo MOCK_ROOT        = "${MOCK_ROOT}"
echo MOCK_CFG_RELEASE = "${MOCK_CFG_RELEASE}"

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

    # Read only clone
    try git clone -q https://github.com/OpenMAMA/OpenMAMA.git > ${BUILD_DIR}/git-clone.log 2>&1
    try cd OpenMAMA
    try git checkout ${BRANCH} > ${BUILD_DIR}/git-checkout.log 2>&1
    try cd ${SOURCE_DIR}

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

    try git clone -q --depth=1 https://github.com/OpenMAMA/OpenMAMA-testdata.git > ${BUILD_DIR}/git-sampledata.log 2>&1
    try mv OpenMAMA-testdata ${DATA_DIR}
    try rm -rf ${DATA_DIR}/.git
    next
    RETURN_CODE=$?
fi

if [ $SRC_RPM_BUILD -eq 1 ] && [ $RETURN_CODE -eq 0 ]; then
    step "Create the source tarball"
    try mv OpenMAMA openmama-${VERSION}
    try cp -r ${DATA_DIR} openmama-${VERSION}
    try tar -zcf openmama-${VERSION}.tgz openmama-${VERSION}
    try cp openmama-${VERSION}.tgz ${BUILD_DIR}/SOURCES/
    next
    step "Perform the Source RPM Build"
    try cd ${BUILD_DIR}/SPECS
    try rpmbuild -bs --define "BUILD_VERSION "${VERSION} --define "BUILD_NUMBER "${BUILD_NUMBER} openmama.spec > ${BUILD_DIR}/rpmbuild.log 2>&1
    next
    RETURN_CODE=$?
fi

if [ $MOCK_BUILD -eq 1 ] && [ $RETURN_CODE -eq 0 ]; then
    try cd ${BUILD_DIR}/SRPMS

    for mockcfg in $ALL_MOCK_CFG
    do
        step "Build multiplatform RPMs for $mockcfg"
        try /usr/bin/mock -r /etc/mock/$mockcfg.cfg --enable-network --define 'BUILD_VERSION '${VERSION} --define 'BUILD_NUMBER '${BUILD_NUMBER} openmama-${VERSION}-${BUILD_NUMBER}.*.src.rpm >> ${BUILD_DIR}/mock-$(basename $mockcfg).log 2>&1
        next
        rc=$?
        # Only set return code if still zero
        if [ 0 = $RETURN_CODE ]
        then
            RETURN_CODE=$rc
        fi
    done
fi

if [ $PACKAGE_RELEASE -eq 1 ] && [ $RETURN_CODE -eq 0 ]; then
    step "Packaging release"

    if [ -d "${RELEASE_DIR}" ]; then
        try rm -rf ${RELEASE_DIR}
    fi

    try mkdir -p ${RELEASE_DIR}

    # Copy in the Source RPM
    try cp ${BUILD_DIR}/SRPMS/openmama-${VERSION}-${BUILD_NUMBER}.*.src.rpm ${RELEASE_DIR}

    # These paths are hard coded, since they should always remain there. We need
    # to update these periodically.
    for mockcfg in $ALL_MOCK_CFG
    do
        try cp $MOCK_ROOT/$mockcfg/result/openmama-${VERSION}-${BUILD_NUMBER}.*.rpm ${RELEASE_DIR}
    done

    # Build and tar the binary release
    if [ -d ${BUILD_DIR}/binary ]; then
        try rm -rf ${BUILD_DIR}/binary
    fi

    BINARY_DIR=${BUILD_DIR}/binary/openmama-${VERSION}
    MOCK_BIN_TARBALL_DIR=$MOCK_ROOT/$MOCK_CFG_RELEASE/root/builddir/build/BUILD/openmama-${VERSION}
    try mkdir -p ${BINARY_DIR}
    try mkdir ${BINARY_DIR}/data
    try mkdir ${BINARY_DIR}/config
    try cp -r ${MOCK_BIN_TARBALL_DIR}/data/* ${BINARY_DIR}/data/
    try cp -r ${MOCK_BIN_TARBALL_DIR}/openmama_install*/* ${BINARY_DIR}/
    try cp    ${MOCK_BIN_TARBALL_DIR}/data/profiles/profile.openmama ${BINARY_DIR}/

    try cd ${BUILD_DIR}/binary/
    try tar -zcf ${RELEASE_DIR}/openmama-${VERSION}.linux.x86_64.tar.gz openmama-${VERSION}

    next
    RETURN_CODE=$?
fi

exit $RETURN_CODE
