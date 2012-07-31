# $Id: Makefile.am,v 1.1.2.9 2011/09/29 09:19:22 emmapollock Exp $
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

# Targets to be installed:
lib_LTLIBRARIES = libmamaavisimpl.la


CPPFLAGS += \
	-I$(srcdir)/../../ \
	-I$(srcdir)/../../../../../../common/c_cpp/src/c 

LDFLAGS += \
    -L../../ \
    -L$(AVIS_HOME)/lib \
    -L$(srcdir)/../../../../../../common/c_cpp/src/c

if USE_GCC_FLAGS
CFLAGS += -Wimplicit -Wno-long-long -Wmissing-prototypes -Wstrict-prototypes -Wall
CPPFLAGS += -Wno-long-long -Wimplicit -Wno-long-long -Wmissing-prototypes -Wstrict-prototypes -Wall
endif

LIBS    = -lavis -lmama -lm -lwombatcommon -luuid

libmamaavisimpl_la_SOURCES = \
	bridge.c \
	msg.c  \
	sub.c  \
	subinitial.c \
	transportbridge.c \
	publisher.c \
	io.c \
	queue.c \
	timer.c

