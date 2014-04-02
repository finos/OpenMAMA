# $Id$
#
# OpenMAMA: The open middleware agnostic messaging API
# Copyright (C) 2011 NYSE Technologies, Inc.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301 USA
#

##################################################
#
# VERSION INFO
#
##################################################
m4_define([product_version_major],     [2])
m4_define([product_version_minor],     [2])
m4_define([product_version_release],   [1.1])
m4_define([product_full_version],[product_version_major.product_version_minor.product_version_release])


##################################################
#
# INTIALISATION
#
##################################################
AC_INIT(mama, [product_full_version], apis@wombatfs.com)
AC_CONFIG_SRCDIR([src/c/mama.c])
AC_CONFIG_SRCDIR([src/cpp/mamacpp.cpp])
AC_CONFIG_AUX_DIR([etc])
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([config.h])
AC_CANONICAL_BUILD()

# Checks for programs.
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_CC

# make sure that we configure bridges or specifically build without them
mwbridge=false
payloadbridge=false

# Check whether to use built-in library archiver if C++ compiler is Sun/Forte
if CXXbase=`echo ${CXX} | ${ac_cv_prog_AWK} '{ print $1; }'` ; \
   test "x${build_vendor}" = "xsun" -a "x`basename ${CXXbase}`" = "xCC"
then
  AR="${CXX} -xar"
  AR_FLAGS="-o"
  LIBS="$LIBS -lrt"
fi

# Use libtool:
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL
AC_SUBST(LTLIBOBJS)

AC_LANG([C])
AC_LANG([C++])

# Default paths.
AC_PREFIX_DEFAULT([/opt/openmama])


# Always put the source's include directory in the -I search path
CPPFLAGS="$CPPFLAGS -I${srcdir}/src/c"


# Flush all cached values to the cache file
AC_CACHE_SAVE


##################################################
#
# PREREQUISITES
#
##################################################

# Check for standard header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Check whether we can use gcc specific compiler flags
AM_CONDITIONAL(USE_GCC_FLAGS, test "x${GCC}" = "xyes")

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([socket])
AC_CHECK_LIB(socket,socket)
AC_CHECK_LIB(nsl,gethostbyname)

# SOLARIS 8 does not have sem_timedwait
AC_SEARCH_LIBS(sem_timedwait, rt pthread,
    [
        echo "Found sem_timedwait()."
    ],
    [
        echo "Failed to find sem_timedwait()."
        CPPFLAGS="$CPPFLAGS -DNO_SEM_TIMEDWAIT"
    ])

# Check whether C++ compiler supports member templates
builtin(include,ac_cxx_member_templates.m4)
AC_CXX_MEMBER_TEMPLATES

##################################################

# Flush all cached values to the cache file
AC_CACHE_SAVE


##################################################
#
# OPTIONS
#
##################################################
##################################################

##################################################
##################################################

##################################################
# ENTITLEMENTS: --with-entitlements
# Whether or not to link in oea entitlements
##################################################
AC_ARG_WITH(
    entitlements,
    [AC_HELP_STRING([--with-entitlements=DIR],
                    [Location of ENTITLEMENTS SRC in DIR])],
    [
     if test -d "$withval"; then
        OEA_HOME="$withval"
     fi
    ]
    AC_CHECK_FILE(
        [$OEA_HOME/include/oea/OeaClient.h],
        [
         CPPFLAGS="$CPPFLAGS -DWITH_ENTITLEMENTS -I$OEA_HOME/include/oea"
         LDFLAGS="$LDFLAGS -L$WOMBATMSG_HOME/lib -L$OEA_HOME/lib"
         LIBS="$LIBS -loeac"
         with_entitlements=true
        ],
        [
         AC_MSG_FAILURE([Building with Entitlement Failed.])
        ])
    )
AC_SUBST(entitlements_dir,[$OEA_HOME])
AM_CONDITIONAL(WITH_ENTITLEMENTS, test x$with_entitlements = "xtrue")

if test x$with_entitlements = xtrue
then
  AC_MSG_CHECKING(for Solaris)
  case "$host" in
    *-solaris*)
      LIBS="$LIBS -lnsl -lsocket"
      ;;
  esac
fi

##################################################
# AVIS: --with-avis
# Whether or not to link in the Avis middleware
##################################################
# Whether to build and where to find the Avis Elvin API headers and
# libraries (usually /usr/local):
AVIS_HOME=[${AVIS_HOME}]
AC_ARG_WITH(
    avis,
    [AC_HELP_STRING([--with-avis=DIR],
                    [Location of Avis API include/library files in DIR])],
    [
     if test -d "$withval"; then
         AVIS_HOME="$withval"
     fi
     AC_CHECK_FILES(
         [$AVIS_HOME/include/avis/avis_client_config.h],
         [
          CPPFLAGS="$CPPFLAGS -I$AVIS_HOME/include"
          LDFLAGS="$LDFLAGS -L$AVIS_HOME/lib"
          with_avis=true
          mama_platform=avis
          mama_payload=avis
          mwbridge=true
          payloadbridge=true
         ],
         [
            AC_MSG_ERROR([Valid Avis installation not found!  You may need to copy avis_client_config.h from platforms/gnu_auotmake to $AVIS_HOME/include/avis)])
         ])
    ])
AM_CONDITIONAL(WITH_AVIS, test x$with_avis = "xtrue")

##################################################
# QPID: --with-proton
# Whether or not to link in the Avis middleware
##################################################
# Whether to build and where to find the Qpid proton API headers and libraries
# (usually /usr/local):
PROTON_HOME=[${PROTON_HOME}]
AC_ARG_WITH(
    proton,
    [AC_HELP_STRING([--with-proton=DIR],
                    [Location of Proton API include/library files in DIR])],
    [
     if test -d "$withval"; then
         PROTON_HOME="$withval"
     fi
     AC_CHECK_FILE(
         [$PROTON_HOME/include/proton/parser.h],
         [
           CFLAGS="$CFLAGS -I$PROTON_HOME/include"
           LDFLAGS="$LDFLAGS -L$PROTON_HOME/lib -L$PROTON_HOME/lib64"
           with_proton=true
           mama_platform=proton
           mama_payload=proton
           mwbridge=true
           payloadbridge=true
         ],
         [
            AC_MSG_ERROR([Valid QPID Proton installation not found! Check $PROTON_HOME/include/proton)])
         ])
    AC_CHECK_FILE(
         [$PROTON_HOME/include/proton/version.h],
         [
           CFLAGS="$CFLAGS -DHAVE_QPID_PROTON_VERSION_H"
         ],[])
    ])
AM_CONDITIONAL(WITH_PROTON, test x$with_proton = "xtrue")

##################################################
##################################################

##################################################
# EXAMPLES: --without-examples
# Whether or not to build the example apps
##################################################
AC_ARG_WITH(
    examples,
    [AC_HELP_STRING([--without-examples],
                    [exclude mama examples])],
    [
     with_examples=false
    ],
    [
     with_examples=true
    ])
AM_CONDITIONAL(WITH_EXAMPLES, test x$with_examples = "xtrue")

##################################################
# UNIT TESTS: --with-unittests
# Whether or not to build the unit tests
# GTEST_HOME is the root directory of the gtest installation. It can be
# specified as an environment variable or as a configure parameter
##################################################
UNIT_TEST_HOME=[$GTEST_HOME]
AC_ARG_WITH(
    unittests,
    [AC_HELP_STRING([--with-unittests=DIR],
                    [Location of the gtest API include/library files in DIR])],
    [
     if test -d $withval; then
         UNIT_TEST_HOME="$withval"
     fi
     CPPFLAGS="$CPPFLAGS -I$UNIT_TEST_HOME/include -I$UNIT_TEST_HOME -DWITH_UNITTESTS "
     AC_CHECK_HEADERS(
         [$UNIT_TEST_HOME/include/gtest/gtest.h],
         [
          LDFLAGS="$LDFLAGS -L$UNIT_TEST_HOME/lib"

      with_unittests=true
         ],
         [
            AC_MSG_ERROR([Valid GTEST installation not found!])
         ]
         )
    ],
    [
     with_unittests=false
    ])
AM_CONDITIONAL(WITH_UNITTESTS, test x$with_unittests = xtrue)

##################################################
# TEST TOOLS: --with-testtools
# Whether or not to build the test tools
##################################################
AC_ARG_WITH(
    testtools,
    [AC_HELP_STRING([--with-testtools],
                    [Build test tools])],
    [
      with_testtools=true
    ])
AM_CONDITIONAL(WITH_TESTTOOLS, test x$with_testtools = xtrue)

##################################################
# NO BRIDGES: --no-middleware-bridge --no-payload-bridge
#
# If we get here and the build is not configured for a bridge we need to ensure
# that we really want to build without them.
#
##################################################

# --without-middleware-bridge
AC_ARG_WITH([middleware-bridge],
	[AS_HELP_STRING([--without-middleware-bridge],
	 [Build OpenMAMA with no middleware bridge.])],
	[],
	[with_middleware_bridge=yes])

# --without-payload-bridge
AC_ARG_WITH([payload-bridge],
	[AS_HELP_STRING([--without-payload-bridge],
	 [Build OpenMAMA with no payload bridge.])],
	[],
	[with_payload_bridge=yes])


# Trying to build without a message bridge but --without-middleware-bridge
# not specified
if test x${mwbridge} = xfalse -a x$with_middleware_bridge = xyes; then
	AC_MSG_ERROR([No middleware bridge specified! If this is intentional specify --without-middleware-bridge])
fi

# Trying bo build without a middleware bridge but bridge specified
if test x${mwbridge} = xtrue -a x$with_middleware_bridge = xno; then
	AC_MSG_ERROR([--without-middleware-bridge specified with ${mama_platform}])
fi

# Trying to build without a payload bridge but --without-payload-bridge
# not specified
if test x${payloadbridge} = xfalse -a x$with_payload_bridge = xyes; then
     AC_MSG_ERROR([No payload bridge specified! If this is intentional specify --without-payload-bridge])
fi

# Trying bo build without a payload bridge but bridge specified
if test x${mwbridge} = xtrue -a x$with_payload_bridge = xno; then
	AC_MSG_ERROR([--without-payload-bridge specified with ${mama_payload}])
fi

##################################################
#
# VERSION FILE GENERATION
#   
##################################################
MAMA_VERSION_MAJOR=product_version_major
MAMA_VERSION_MINOR=product_version_minor
MAMA_VERSION_RELEASE=product_version_release

#Is this a DEVRelease
DEV_BUILD=1
if (test "`groups | grep nysevalidbuilders`" = "" || (test "${REL_BUILD}" && test "${REL_BUILD}" != 0 )); then
DEV_BUILD=0
fi

if (test "${DEV_BUILD}" = "0") ; then
PACKAGE_VERSION="DEVRELEASE${MAMA_VERSION_MAJOR}.${MAMA_VERSION_MINOR}.${MAMA_VERSION_RELEASE}"
fi

AM_CONDITIONAL(DEV_RELEASE, [test "$DEV_BUILD" = "0" ])
mkdir -p src/c/mama
echo "/* This file was automatically generated */" >  src/c/mama/version.h
echo "#ifndef MamaVersionH"                        >> src/c/mama/version.h
echo "#define MamaVersionH"                        >> src/c/mama/version.h
echo ""                                            >> src/c/mama/version.h
echo "#define MAMA_VERSION_MAJOR ${MAMA_VERSION_MAJOR}"     >> src/c/mama/version.h
echo "#define MAMA_VERSION_MINOR ${MAMA_VERSION_MINOR}"     >> src/c/mama/version.h
echo "#define MAMA_VERSION_RELEASE ${MAMA_VERSION_RELEASE}" >> src/c/mama/version.h
echo "#define MAMA_VERSION \"openmama ${PACKAGE_VERSION}\""     >> src/c/mama/version.h
echo "extern const char* mama_version;"            >> src/c/mama/version.h
echo ""                                            >> src/c/mama/version.h
echo "#endif"                                      >> src/c/mama/version.h

# Ensure documents have correct version number
cat ${srcdir}/doxyconfig-c.in > ${srcdir}/doxyconfig-c
echo "PROJECT_NUMBER = ${PACKAGE_VERSION}" >> ${srcdir}/doxyconfig-c
cat ${srcdir}/doxyconfig-cpp.in > ${srcdir}/doxyconfig-cpp
echo "PROJECT_NUMBER = ${PACKAGE_VERSION}" >> ${srcdir}/doxyconfig-cpp

##################################################
#
# FINAL OUTPUT
#
##################################################
AC_OUTPUT([Makefile \
    src/Makefile \
    src/c/Makefile \
    src/c/bridge/Makefile \
    src/c/bridge/avis/Makefile \
    src/c/bridge/qpid/Makefile \
    src/c/payload/Makefile \
    src/c/payload/avismsg/Makefile \
    src/c/payload/qpidmsg/Makefile \
    src/cpp/Makefile \
    src/examples/Makefile \
    src/examples/c/Makefile \
    src/examples/cpp/Makefile \
    src/testtools/Makefile \
    src/testtools/capturereplay/Makefile \
    src/testtools/capturereplay/c/Makefile \
    src/testtools/performance/Makefile \
    src/testtools/performance/c/Makefile \
    src/testtools/performance/cpp/Makefile \
    src/testtools/load/Makefile \
    src/testtools/load/c/Makefile \
    src/testtools/load/cpp/Makefile \
    src/gunittest/Makefile \
    src/gunittest/c/Makefile \
    src/gunittest/c/payload/Makefile \
    src/gunittest/c/middleware/Makefile \
    src/gunittest/c/fieldcache/Makefile \
    src/gunittest/c/mamamsg/Makefile \
    src/gunittest/c/mamaprice/Makefile \
    src/gunittest/c/mamadatetime/Makefile \
    src/gunittest/cpp/Makefile \
    src/gunittest/cpp/fieldcache/Makefile \
    ])
