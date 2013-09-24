# $Id: Makefile.am,v 1.1.2.9 2011/09/29 09:19:22 emmapollock Exp $
#
# OpenMAMA: The open middleware agnostic messaging API
# Copyright (C) 2011 NYSE Inc.
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
lib_LTLIBRARIES = libmamaqpidimpl.la


CPPFLAGS += \
    -I$(srcdir)/../../ \
    -I$(srcdir)/../../../../../../common/c_cpp/src/c \
    -I$(QPID_HOME)/include/ 

LDFLAGS += \
    -L../../ \
    -L$(srcdir)/../../../../../../common/c_cpp/src/c \
    -L$(QPID_HOME)/proton-c

if USE_GCC_FLAGS
CFLAGS += -Wimplicit -Wno-long-long -Wmissing-prototypes -Wstrict-prototypes -Wall
CPPFLAGS += -Wno-long-long -Wimplicit -Wno-long-long -Wmissing-prototypes -Wstrict-prototypes -Wall
endif

LIBS    =  -luuid -lmama -lm -lwombatcommon -lqpid-proton

libmamaqpidimpl_la_SOURCES = \
	bridge.c \
	transport.c \
	queue.c \
	publisher.c \
	subscription.c \
	msg.c \
	io.c \
	timer.c \
	inbox.c \
	codec.c \
	endpointpool.c
 

