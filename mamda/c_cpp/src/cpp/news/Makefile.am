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

lib_LTLIBRARIES = libmamdanews.la

nobase_include_HEADERS  = \
	mamda/MamdaNewsFields.h \
	mamda/MamdaNewsHeadline.h \
	mamda/MamdaNewsHeadlineHandler.h \
	mamda/MamdaNewsManager.h \
	mamda/MamdaNewsMetaData.h \
	mamda/MamdaNewsQuery.h \
	mamda/MamdaNewsQueryHandler.h \
	mamda/MamdaNewsStory.h \
	mamda/MamdaNewsStoryHandler.h \
	mamda/MamdaNewsTypes.h

CPPFLAGS += -I$(srcdir)/.. \
            -I$(srcdir)/../../../../../mama/c_cpp/src/cpp/mama \
            -I$(srcdir)/../../../../../mama/c_cpp/src/cpp \
            -I$(srcdir)/../../../../../mama/c_cpp/src/c \
            -I$(srcdir)/../../../../../common/c_cpp/src/c 

if USE_GCC_FLAGS
CFLAGS += -pedantic -Wno-long-long -Wall
CPPFLAGS += -pedantic -Wno-long-long -Wall
endif

libmamdanews_la_SOURCES = \
        MamdaNewsFields.cpp \
        MamdaNewsHeadline.cpp \
        MamdaNewsManager.cpp \
        MamdaNewsStory.cpp \
        MamdaNewsUtils.cpp
