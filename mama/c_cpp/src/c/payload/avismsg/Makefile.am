# $Id: Makefile.am,v 1.1.2.5 2011/09/27 11:39:48 emmapollock Exp $
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
blddir = @builddir@
VPATH  = @srcdir@

# Targets to be installed:

lib_LTLIBRARIES = libmamaavismsgimpl.la

INCLUDES = -I${srcdir}/../../ \
           -I${srcdir}/../../../../../../common/c_cpp/src/c

CPPFLAGS += \
	-I$(srcdir)/../../

if USE_GCC_FLAGS
CFLAGS += -Wimplicit -Wno-long-long -Wmissing-prototypes -Wstrict-prototypes -Wall
CPPFLAGS += -Wno-long-long -Wimplicit -Wno-long-long -Wmissing-prototypes -Wstrict-prototypes -Wall
endif

LDFLAGS  += -L${srcdir}/../../ \
            -L${srcdir}/../../../../../../common/c_cpp/src/c

LIBS    = -lavis -lmama -lm

# Sources:
libmamaavismsgimpl_la_SOURCES = \
	avispayload.c  \
	avismsgimpl.c  \
	msgfield.c
