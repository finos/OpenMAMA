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


CPPFLAGS += -I$(srcdir)/../../cpp \
            -I$(srcdir)/../../cpp/orderbooks \
            -I$(srcdir)/../../cpp/options \
            -I$(srcdir)/../../../../../mama/c_cpp/src/c \
            -I$(srcdir)/../../../../../mama/c_cpp/src/cpp \
            -I$(srcdir)/../../../../../common/c_cpp/src/c \
            -I$(srcdir)/../.. \
            -I$(srcdir)/.. \
            -I/usr/local/include

LDFLAGS  += -L${srcdir}/../../cpp \
            -L$(srcdir)/../../cpp/orderbooks \
            -L$(srcdir)/../../../../../mama/c_cpp/src/c \
            -L$(srcdir)/../../../../../mama/c_cpp/src/cpp

if USE_GCC_FLAGS
 LDFLAGS += -rdynamic
endif

LIBS += -lmamdabook -lmamda -lmamacpp -lmama -lpthread -lnsl -ldl -lm

if WITH_NCURSES
bookviewer_LDADD = -lncurses
endif

bin_PROGRAMS = atomicbookbuilder \
               atomicbookticker \
			   bookticker \
			   bookpublisher
if WITH_NCURSES
bin_PROGRAMS += bookviewer 
endif
if WITH_TESTBED
bin_PROGRAMS += bookchurn
endif


common_files = \
	$(srcdir)/../parsecmd.cpp \
	$(srcdir)/../dictrequester.cpp \
    $(srcdir)/../parsecmd.h \
    $(srcdir)/../dictrequester.h   

dist_atomicbookticker_SOURCES     = atomicbookticker.cpp    $(common_files)
dist_atomicbookbuilder_SOURCES    = atomicbookbuilder.cpp    $(common_files)
dist_bookticker_SOURCES           = bookticker.cpp          $(common_files)
dist_bookpublisher_SOURCES        = bookpublisher.cpp       $(common_files)
if WITH_NCURSES
dist_bookviewer_SOURCES           = bookviewer.cpp          $(common_files)
endif
if WITH_TESTBED
dist_bookchurn_SOURCES        = bookchurn.cpp           $(common_files)
endif

# Additional installation directory for sample source code
exampledir = ${prefix}/examples/mamda/cpp/orderbook

# Rule to install the example programs:
# Always distribute bookviewer even if we did not build it.
# The customers may be able to compile if they have ncurses.
example_DATA = \
        atomicbookticker.cpp \
        atomicbookbuilder.cpp \
        bookticker.cpp \
        bookpublisher.cpp \
        bookchurn.cpp \
        $(common_files) \
        Makefile.sample

if WITH_NCURSES
example_DATA += bookviewer.cpp
endif
