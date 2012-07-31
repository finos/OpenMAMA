# $Id: Makefile.am,v 1.14.2.8.2.1.4.6 2011/08/23 14:29:46 emmapollock Exp $
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

lib_LTLIBRARIES = libwombatcommon.la

CFLAGS   += -std=gnu99 -Wall \
        -Wno-long-long -Wno-unused-parameter -Wdeclaration-after-statement
CXXFLAGS += -Wall \
        -Wno-long-long -Wno-unused-parameter -Wno-write-strings

nobase_include_HEADERS = \
    destroyHandle.h \
    list.h \
    lookup2.h \
    platform.h \
    property.h \
    wlock.h \
    timers.h \
    wombat/port.h \
    wombat/MRSWLock.h \
    wombat/environment.h \
    wombat/machine.h \
    wombat/queue.h \
    wombat/strutils.h \
    wombat/wCommon.h \
    wombat/wConfig.h \
    wombat/wInterlocked.h \
    wombat/wSemaphore.h \
    wombat/wincompat.h \
    wombat/wMessageStats.h \
    wombat/wtable.h \
    wombat/wUuid.h

if USE_GCC_FLAGS
CPPFLAGS += -pedantic -Wno-long-long -D_GNU_SOURCE -fPIC 
endif

LIBS     += -ldl -luuid

libwombatcommon_la_SOURCES = \
    MRSWLock.c \
    timers.c \
    destroyHandle.c \
    environment.c \
    libyywrap.c \
    list.c \
    lookup2.c \
    machine.c \
    network.c \
    machine_port.c \
    platform.c \
    properties.l \
    property.c \
    queue.c \
    strutils.c \
    wSemaphore.c \
    wlock.c \
    wMessageStats.c \
    wtable.c
