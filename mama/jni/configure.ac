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


##################################################
#
# VERSION INFO
#
##################################################
m4_define([product_version_major],     [6])
m4_define([product_version_minor],     [0])
m4_define([product_version_release],   [0rc17])
m4_define([product_full_version],[product_version_major.product_version_minor.product_version_release])


##################################################
#
# INTIALISATION
#
##################################################
AC_INIT(mamajni, [product_full_version], support@wombatfs.com)
AC_CONFIG_SRCDIR([src/c/mamajni.c])
AC_CONFIG_AUX_DIR([etc])
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([config.h])
AC_CANONICAL_BUILD()

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

#Check whether to build for 64bit
AM_CONDITIONAL(BLDJNI64BIT, test "`uname -m`" = "x86_64")

if test "`uname -s`" = "SunOS";
then
    AM_CONDITIONAL(BLDJNI64BIT, test "`isainfo -b`" = "64")
fi

# Use libtool:
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL
AC_SUBST(LTLIBOBJS)

AC_LANG([C])
AC_LANG([C++])

# Default paths.
AC_PREFIX_DEFAULT([/opt/openmama])

CPPFLAGS="$CPPFLAGS -I$JAVA_HOME/include"
CPPFLAGS="$CPPFLAGS -I$JAVA_HOME/include/solaris"
CPPFLAGS="$CPPFLAGS -I$JAVA_HOME/include/linux"
LDFLAGS="$LDFLAGS -L$JAVA_HOME/lib"

# Flush all cache values to the cache file
AC_CACHE_SAVE


##################################################
#
# PREREQUISITES
#
##################################################
# Checks for standard header files.
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


##################################################
#
# OPTIONS
#
##################################################
# Where to find the MAMA headers and libraries
MAMA_HOME=${MAMA_HOME:-/opt/openmama}
AC_ARG_WITH(
    mama,
    [AC_HELP_STRING([--with-mama=DIR],
                    [Use MAMA include/library files in DIR])],
    [
     if test -d "$withval"; then 
         MAMA_HOME="$withval" 
     else
         AC_MSG_FAILURE([Cannot find directory: $withval])
     fi
    ])
CPPFLAGS="$CPPFLAGS -I$MAMA_HOME/include"
LDFLAGS="$LDFLAGS -L$MAMA_HOME/lib"
LIBS="$LIBS -lmama"


##################################################
#
# VERSION FILE GENERATION
#   
##################################################
mkdir -p src/c
echo "#ifndef MamaJniVersionH" >  src/c/version.h
echo "#define MamaJniVersionH" >> src/c/version.h
echo "extern const char mamajni_version[];" >> src/c/version.h
echo "#define MAMAJNI_VERSION() { const char* version = \"@(#) openmamajni ${PACKAGE_VERSION}\"; }" >> src/c/version.h
echo "#endif" >> src/c/version.h


##################################################
#
# FINAL OUTPUT
#
##################################################
AC_OUTPUT([Makefile \
    src/Makefile \
	src/c/Makefile])
