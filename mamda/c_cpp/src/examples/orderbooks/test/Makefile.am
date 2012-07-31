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

bin_PROGRAMS = \
	bookselftest \
	randomaccess \
	bookiters

INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../..
LDFLAGS  = -L.. -L../..

#Will this work on solaris?
if STATIC_LINK_EXAMPLES
  LDFLAGS += -static
endif

LIBS = -lmamdabook -lmamda -lmamacpp -lrt 
LIBS += -lmama -lwombatcommon

common_files = \
        Makefile.sample \
        $(srcdir)/../../parsecmd.cpp \
        $(srcdir)/../../dictrequester.cpp

bookselftest_SOURCES = bookselftest.cpp $(common_files)
randomaccess_SOURCES = randomaccess.cpp $(common_files)
bookiters_SOURCES    = bookiters.cpp    $(common_files)
