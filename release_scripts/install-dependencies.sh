#!/bin/bash

# Make all unhandled erroneous return codes fatal
set -e

# Globals
export SDKMAN_DIR=${SDKMAN_DIR:-/usr/local/sdkman}
VERSION_GTEST=${VERSION_GTEST:-1.8.0}
VERSION_APR=${VERSION_APR:-1.6.3}
VERSION_QPID=${VERSION_QPID:-0.24.0}
DEPS_DIR=${DEPS_DIR:-/app/deps}

# Constants
FEDORA=Fedora
RHEL=CentOS
UBUNTU=Ubuntu

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
    if [ -f /etc/fedora-release ]
    then
        DISTRIB_ID=$FEDORA
    else
        DISTRIB_ID=$RHEL
    fi
fi

if [ -f /etc/lsb-release ]
then
    # Will set DISTRIB_ID and DISTRIB_RELEASE
    source /etc/lsb-release
fi

if [ -z "$DISTRIB_RELEASE" ]
then
    echo "Unsupported distro found: $(cat /etc/*-release)" && exit $LINENO
fi

if [ "$DISTRIB_ID" = "$FEDORA" ]
then
    echo "Installing fedora specific dependencies"
    yum install -y libnsl2-devel libffi-devel ruby-devel rubygems redhat-rpm-config rpm-build
fi

if [ "$DISTRIB_ID" = "$RHEL" ]
then
    echo "Installing CentOS / RHEL specific dependencies"
    yum install -y epel-release gcc make rpm-build which
fi

if [ "$DISTRIB_ID" = "$RHEL" ] || [ "$DISTRIB_ID" = "$FEDORA" ]
then
    yum install -y zlib-devel openssl-devel zip unzip make \
	    java-1.8.0-openjdk-devel libuuid-devel flex doxygen \
	    qpid-proton-c-devel libevent-devel ncurses-devel \
	    apr-devel wget curl cmake gcc-c++ libuuid qpid-proton-c \
	    libevent ncurses apr valgrind python
fi

# General ubuntu packages
if [ "$DISTRIB_ID" = "$UBUNTU" ]
then
    apt-get update -qq
    apt-get install -y ruby ruby-dev build-essential \
	    zip unzip curl git flex uuid-dev libevent-dev \
	    cmake git libzmq3-dev ncurses-dev \
	    unzip valgrind libapr1-dev python libz-dev
fi

# Ubuntu 18 specific software
if [ "$DISTRIB_ID" = "$UBUNTU" ] && [ "${DISTRIB_RELEASE:0:2}" = "18" ]
then
    apt-get install -y rubygems openjdk-8-jdk
fi

# Ubuntu 16 specific software
if [ "$DISTRIB_ID" = "$UBUNTU" ] && [ "${DISTRIB_RELEASE:0:2}" = "16" ]
then
    apt-get install -y openjdk-8-jdk libssl-dev
    echo "export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64" > /etc/profile.d/profile.jni.sh
fi

# Ubuntu 16 specific software
if [ "$DISTRIB_ID" = "$UBUNTU" ] && [ "${DISTRIB_RELEASE:0:2}" = "14" ]
then
    apt-get install -y openjdk-7-jdk libssl-dev
    echo "export JAVA_HOME=/usr/lib/jvm/java-7-openjdk-amd64" > /etc/profile.d/profile.jni.sh
fi

test -d $DEPS_DIR || mkdir -p $DEPS_DIR

# Centos and old ubuntu version specific dependencies (ruby is too old for FPM)
if [[ ("$DISTRIB_ID" = "$RHEL" && "${DISTRIB_RELEASE:0:1}" = "6") || ("$DISTRIB_ID" = "$UBUNTU" && "${DISTRIB_RELEASE:0:2}" != "18") ]]
then
    cd $DEPS_DIR
    curl -sL http://cache.ruby-lang.org/pub/ruby/2.1/ruby-2.1.2.tar.gz | tar xz
    cd ruby-2.1.2
    ./configure --prefix=/usr
    make
    make install && gem update --system
elif [ "$DISTRIB_ID" = "$RHEL" ]
then
    yum install -y ruby-devel rubygems
fi

# Install gradle
curl -s "https://get.sdkman.io" | bash
source "$SDKMAN_DIR/bin/sdkman-init.sh"
sdk install gradle

# Install FPM for packaging up
gem install --no-ri --no-rdoc fpm

# Gtest is best always getting built
cd $DEPS_DIR
curl -sL http://github.com/google/googletest/archive/release-$VERSION_GTEST.tar.gz | tar xz
cd googletest-release-$VERSION_GTEST
mkdir bld
cd bld
cmake -DCMAKE_INSTALL_PREFIX=/usr .. && make && make install

# Ubuntu doesn't have this as a package yet so need to build
if [ "$DISTRIB_ID" = "$UBUNTU" ]
then
    cd $DEPS_DIR
    curl -sL http://github.com/apache/qpid-proton/archive/$VERSION_QPID.tar.gz | tar xz
    cd qpid-proton-$VERSION_QPID
    mkdir bld
    cd bld
    cmake -DBUILD_TESTING:BOOL=OFF -DBUILD_CPP:BOOL=OFF -DCMAKE_INSTALL_PREFIX=/usr ..
    make
    make install
fi
