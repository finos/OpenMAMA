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
m4_define([product_version_major],     [6])
m4_define([product_version_minor],     [0])
m4_define([product_version_release],   [0rc19])
m4_define([product_full_version],[product_version_major.product_version_minor.product_version_release])


##################################################
#
# INTIALISATION
#
##################################################
AC_INIT(mamda, [product_full_version], apis@wombatfs.com)
AC_CONFIG_SRCDIR([src/cpp/MamdaSubscription.cpp])
AC_CONFIG_AUX_DIR([etc])
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([config.h])
AC_CANONICAL_BUILD()


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Check whether to use built-in library archiver if C++ compiler is Sun/Forte
if CXXbase=`echo ${CXX} | ${ac_cv_prog_AWK} '{ print $1; }'` ; \
   test "x${build_vendor}" = "xsun" -a "x`basename ${CXXbase}`" = "xCC"
then
  AR="${CXX} -xar"
  AR_FLAGS="-o"
fi

# Use libtool:
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL
AC_SUBST(LTLIBOBJS)

AC_LANG([C])
AC_LANG([C++])

# Default paths.
AC_PREFIX_DEFAULT([`pwd`/${PACKAGE_NAME}_${PACKAGE_VERSION}])

# Perform compilation tests using C++
AC_CACHE_SAVE


# Building on Sun/Solaris platform ???
if test "x`uname -s`" = "xSunOS";
then
    CPPFLAGS="$CPPFLAGS -D_SOLARIS_"
fi

# Whether to compile the implementation of the MamdaLock code
# find out if compiling on solaris10 
if test "`uname -s``uname -r |  awk -F"." '{ print $1"."$2 }'`" = "SunOS5.10";
then
    sol10=true 
fi
AM_CONDITIONAL(SOLARIS10, test x$sol10 = "xtrue")


##################################################
#
# PREREQUISITES
#
##################################################

# Check for standard header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h limits.h values.h])

# Check whether we can use gcc specific compiler flags
AM_CONDITIONAL(USE_GCC_FLAGS, test "x${GCC}" = "xyes")

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([socket])

# ncurses is required to build bookviewer
AC_CHECK_LIB(ncurses, tputs, [with_ncurses=true],[AC_MSG_WARN([ncurses not found!])])
AM_CONDITIONAL(WITH_NCURSES, test x$with_ncurses = "xtrue")


##################################################
# WOMBATCOMMON: --with-wombatcommon
# Whether or not to link in wombatcommon
##################################################
WOMBATCOMMON_HOME=[${WOMBATCOMMON_HOME:-/opt/openmama}]
AC_ARG_WITH(
    wombatcommon,
    [AC_HELP_STRING([--with-wombatcommon=DIR],
                    [Location of WombatCommon API include/library files in DIR])],
    [
     if test -d "$withval"; then
         WOMBATCOMMON_HOME="$withval"
     fi
    ])
SAVE_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS -I$WOMBATCOMMON_HOME/include"
AC_CHECK_HEADER(
    [$WOMBATCOMMON_HOME/include/wombat/machine.h],
    [
     SAVE_CPPFLAGS="$SAVE_CPPFLAGS -I$WOMBATCOMMON_HOME/include"
     LDFLAGS="$LDFLAGS -L$WOMBATCOMMON_HOME/lib"
     LIBS="$LIBS -lwombatcommon"
     with_wombat_common=true
    ],
    [
     AC_MSG_WARN([Not using WombatCommon API.])
    ])
AM_CONDITIONAL(WITH_WOMBAT_COMMON, test x$with_wombat_common = "xtrue")

##################################################
# MAMA: --with-mama
# Where to find MAMA
##################################################


##################################################
# TESTBED: --with-testbed
# Whether or not to build the testbed
##################################################
AC_ARG_WITH(
    testbed,
    [AC_HELP_STRING([--with-testbed],
                    [includes testbed programs])],
    [
        with_testbed=true
    ])
AM_CONDITIONAL(WITH_TESTBED, test x$with_testbed = "xtrue")


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
     CPPFLAGS="$CPPFLAGS -I$UNIT_TEST_HOME/include -I$UNIT_TEST_HOME -DWITH_UNITTESTS"
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
#
# VERSION FILE GENERATION
#   
##################################################
MAMDA_VERSION_MAJOR=product_version_major
MAMDA_VERSION_MINOR=product_version_minor
MAMDA_VERSION_RELEASE=product_version_release

mkdir -p src/cpp
echo "/* This file was automatically generated */" >  src/cpp/version.h
echo "#ifndef MamdaVersionAutoGenH"                >> src/cpp/version.h
echo "#define MamdaVersionAutoGenH"                >> src/cpp/version.h
echo ""                                            >> src/cpp/version.h
echo "#define MAMDA_VERSION_MAJOR ${MAMDA_VERSION_MAJOR}"     >> src/cpp/version.h
echo "#define MAMDA_VERSION_MINOR ${MAMDA_VERSION_MINOR}"     >> src/cpp/version.h
echo "#define MAMDA_VERSION_RELEASE ${MAMDA_VERSION_RELEASE}" >> src/cpp/version.h
echo "#define MAMDA_VERSION \"mamda ${PACKAGE_VERSION}\""     >> src/cpp/version.h
echo "extern const char* mamda_version;"           >> src/cpp/version.h
echo ""                                            >> src/cpp/version.h
echo "#endif"                                      >> src/cpp/version.h

# Ensure documents have correct version number
cat ${srcdir}/doxyconfig-cpp.in > ${srcdir}/doxyconfig-cpp
echo "PROJECT_NUMBER = ${PACKAGE_VERSION}" >> ${srcdir}/doxyconfig-cpp


##################################################
#
# FINAL OUTPUT
#
##################################################

AC_OUTPUT([Makefile
    src/Makefile
    src/cpp/Makefile
    src/cpp/news/Makefile
    src/cpp/options/Makefile
    src/cpp/orderbooks/Makefile
    src/examples/Makefile
    src/examples/news/Makefile
    src/examples/orderbooks/Makefile
	src/gunittest/Makefile
    src/gunittest/quotes/Makefile
    src/gunittest/trades/Makefile
    src/gunittest/orderbooks/Makefile
    src/gunittest/secstatus/Makefile
    src/gunittest/orderImbalances/Makefile
    src/testtools/Makefile
    ])
