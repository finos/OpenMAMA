#!/bin/bash

# Make all unhandled erroneous return codes fatal
set -e -x

# Globals
export SDKMAN_DIR=${SDKMAN_DIR:-/usr/local/sdkman}
VERSION_GTEST=${VERSION_GTEST:-1.8.0}
VERSION_APR=${VERSION_APR:-1.6.3}
VERSION_QPID=${VERSION_QPID:-0.37.0}
DEPS_DIR=${DEPS_DIR:-/app/deps}

# Constants
RHEL=CentOS
UBUNTU=Ubuntu

# Initial setup
test -d "$DEPS_DIR" || mkdir -p "$DEPS_DIR"

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
    DISTRIB_RELEASE=$(tr " " "\n" < /etc/redhat-release | grep -E "^[0-9]")
    if [ -f /etc/fedora-release ]
    then
        echo "Fedora builds no longer supported: $(cat /etc/*-release)" && exit $LINENO
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

if [ "$DISTRIB_ID" = "$RHEL" ]
then
    echo "Installing CentOS / RHEL specific dependencies"
    yum install -y epel-release gcc make rpm-build which wget cmake libicu
    if [ "${DISTRIB_RELEASE:0:1}" = "7" ]
    then
        # CentOS 7 cmake version is too old - upgrade it
        (cd /usr && wget -c https://github.com/Kitware/CMake/releases/download/v3.19.4/cmake-3.19.4-Linux-x86_64.tar.gz -O - | tar -xz  --strip-components 1)
    elif [ "${DISTRIB_RELEASE:0:1}" = "8" ]
    then
        # CentOS 8 has funnies around where to find doxygen
        yum install -y dnf-plugins-core
        dnf config-manager --set-enabled powertools
    elif [ "${DISTRIB_RELEASE:0:1}" = "9" ]
    then
        # We want "full" curl not minimal curl and rexml required to work around:
        #     https://github.com/jordansissel/fpm/issues/1784
        dnf install -y --allowerasing curl rubygem-rexml
    fi
    yum install -y python3 zlib-devel openssl-devel zip unzip make \
	    java-11-openjdk-devel libuuid-devel flex doxygen \
	    libevent-devel ncurses-devel python3-pip \
	    apr-devel wget curl gcc-c++ libuuid \
	    libevent ncurses apr apr-util-devel valgrind which git \
	    ruby-devel rubygems
fi

# General ubuntu packages
if [ "$DISTRIB_ID" = "$UBUNTU" ]
then
    export DEBIAN_FRONTEND=noninteractive
    apt-get update
    apt-get install -y ruby ruby-dev build-essential \
	    zip unzip curl git flex uuid-dev libevent-dev \
	    cmake git libzmq3-dev ncurses-dev python3-pip \
	    unzip valgrind libapr1-dev python3 libz-dev wget \
	    apt-transport-https ca-certificates libaprutil1-dev
    apt-get install -y rubygems openjdk-11-jdk
    echo "export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64" > /etc/profile.d/profile.jni.sh
fi

# RHEL 7 ruby is too old - need a more recent version for FPM to work later
if [[ ("$DISTRIB_ID" = "$RHEL" && "${DISTRIB_RELEASE:0:1}" -le "7") ]]
then
    cd "$DEPS_DIR"
    curl -sL "https://cache.ruby-lang.org/pub/ruby/2.6/ruby-2.6.1.tar.gz" | tar xz
    cd ruby-2.6.1
    ./configure --prefix=/usr
    make
    make install && gem update --system
fi

# Install gradle
cd "$DEPS_DIR"
curl -s "https://get.sdkman.io" | bash
source "$SDKMAN_DIR/bin/sdkman-init.sh"
# Stick to gradle 6.9 - 7.x seems to have moved maven libraries around
sdk install gradle 6.9

# Install FPM for packaging up
gem install -N fpm -v 1.12.0

# Gtest is best always getting built
cd "$DEPS_DIR"
curl -sL "http://github.com/google/googletest/archive/release-$VERSION_GTEST.tar.gz" | tar xz
cd "googletest-release-$VERSION_GTEST"
mkdir bld
cd bld
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j
make install

# Best installing qpid proton known version too
cd "$DEPS_DIR"
curl -sL "https://github.com/apache/qpid-proton/archive/refs/tags/$VERSION_QPID.tar.gz" | tar xz
cd "qpid-proton-$VERSION_QPID"
mkdir bld
cd bld
cmake -DSSL_IMPL=none -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j
make install

# Install dotnet
wget --no-check-certificate -q -O /tmp/dotnet-install.sh https://dot.net/v1/dotnet-install.sh
chmod a+x /tmp/dotnet-install.sh
/tmp/dotnet-install.sh -i /usr/local/bin
/tmp/dotnet-install.sh -i /usr/local/bin -c 3.1

# Install cloudsmith CLI
python3 -m pip install --upgrade cloudsmith-cli

# Clean up dependency directory to keep size down
test -d "$DEPS_DIR" && rm -rf "${DEPS_DIR:?}/"*
