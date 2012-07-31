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
srcdir = @srcdir@
VPATH  = @srcdir@
PACKAGE_VERSION = @PACKAGE_VERSION@

lib_LTLIBRARIES = libmamajni.la

if USE_GCC_FLAGS
CFLAGS   += -pedantic -Wno-long-long
CPPFLAGS += -pedantic -Wno-long-long
endif


LDADD   =  -lmama -lwombatcommon -lrt
libmamajni_la_LIBADD = -lmama -lwombatcommon -lrt

CFLAGS  += -I${srcdir}/../../../../common/c_cpp/src/c \
           -I${srcdir}/../../../../mama/c_cpp/src/c

LDFLAGS += -L${srcdir}/../../../../common/c_cpp/src/c \
           -L${srcdir}/../../../../mama/c_cpp/src/c


if BLDJNI64BIT
#Whether to build JNI on 64 bit m/c
CPPFLAGS += -D_64_BIT_PLATFORM
endif


# Sources:
libmamajni_la_SOURCES = \
    version.c mamajni.c \
	mamaconnectionjni.c \
    mamatransportjni.c  \
    mamabridgejni.c \
    mamasubscriptionjni.c \
    mamabasicsubscriptionjni.c \
    mamamsgjni.c \
    mamamsgfieldjni.c \
    mamatimerjni.c \
    mamadictionaryjni.c \
    mamaqueuejni.c \
    mamadispatcherjni.c \
    mamapublisherjni.c \
    mamadqpublisherjni.c \
    mamadqpublishermanagerjni.c \
    mamainboxjni.c \
    mamajniutils.c \
    mamapricejni.c \
    mamadatetimejni.c \
    mamasourcejni.c \
	mamaftmemberjni.c \
    subscriptioncommon.c \
    mamaloghandler.c \
	mamapayloadbridgejni.c

version.c:
	echo "const char mamajni_version[] = \"@(#) openmamajni $(PACKAGE_VERSION)\";" > version.c
