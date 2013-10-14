# $Id: Makefile.am,v 1.1.2.1 2012/11/19 12:04:42 matthewmulhern Exp $
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

if USE_GCC_FLAGS
CFLAGS   += -pedantic -Wno-long-long -O2 -pthread -fPIC
CPPFLAGS += -pedantic -Wno-long-long -O2 -pthread -fPIC
endif

INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../.. -I$(srcdir)/../../../../../../common/c_cpp/src/c -I$(srcdir)/../../../c

CFLAGS   += -I@builddir@/../../c
CPPFLAGS += -I@builddir@/../../c
LDFLAGS  += -L${srcdir}/../../c \
            -L${srcdir}/../../../../../../common/c_cpp/src/c \
            -L${srcdir}/../../../c

LIBS = -lmama -lwombatcommon -lgtest -lpthread
LDADD = -lgtest -ldl

bin_PROGRAMS = UnitTestMamaMiddlewareC

nodist_UnitTestMamaMiddlewareC_SOURCES =  ../MainUnitTestC.cpp \
					middlewareGeneralTests.cpp \
					middlewareIoTests.cpp \
					middlewareMsgTests.cpp \
					middlewareInboxTests.cpp 
