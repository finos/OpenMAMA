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


CPPFLAGS  +=-I$(srcdir)/../../cpp \
            -I$(srcdir)/../../cpp/news \
            -I$(srcdir)/../../cpp/options \
            -I$(srcdir)/../../../../../mama/c_cpp/src/c \
            -I$(srcdir)/../../../../../mama/c_cpp/src/cpp \
            -I$(srcdir)/../../../../../common/c_cpp/src/c \
            -I/usr/local/include

LDFLAGS  += -L${srcdir}/../../cpp \
            -L$(srcdir)/../../cpp/news \
            -L$(srcdir)/../../../../../mama/c_cpp/src/c \
            -L$(srcdir)/../../../../../mama/c_cpp/src/cpp

LIBS += -lmamdanews -lmamda -lmamacpp -lmama -lpthread -lnsl -ldl -lm

bin_PROGRAMS = newsticker

common_files = \
	$(srcdir)/../dictrequester.cpp \
	$(srcdir)/../parsecmd.cpp \
    $(srcdir)/../parsecmd.h \
    $(srcdir)/../dictrequester.h   

dist_newsticker_SOURCES = newsticker.cpp $(common_files)

# Additional installation directory for sample source code
exampledir = ${prefix}/examples/mamda/cpp/news

# Rule to install the example programs:
example_DATA = newsticker.cpp 
