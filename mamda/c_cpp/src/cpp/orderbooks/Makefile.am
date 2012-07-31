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

lib_LTLIBRARIES = libmamdabook.la


nobase_include_HEADERS  = \
	mamda/MamdaBookAtomicGap.h \
	mamda/MamdaBookAtomicLevel.h \
	mamda/MamdaBookAtomicLevelEntry.h \
	mamda/MamdaBookAtomicLevelHandler.h \
	mamda/MamdaBookAtomicLevelEntryHandler.h \
	mamda/MamdaBookAtomicListener.h \
	mamda/MamdaBookAtomicBookHandler.h \
	mamda/MamdaOrderBook.h \
	mamda/MamdaOrderBookBasicDelta.h \
	mamda/MamdaOrderBookBasicDeltaList.h \
	mamda/MamdaOrderBookComplexDelta.h \
	mamda/MamdaOrderBookChecker.h \
	mamda/MamdaOrderBookCheckerHandler.h \
	mamda/MamdaOrderBookCheckType.h \
	mamda/MamdaOrderBookClear.h \
	mamda/MamdaOrderBookConcreteComplexDelta.h \
	mamda/MamdaOrderBookConcreteSimpleDelta.h \
	mamda/MamdaOrderBookDelta.h \
	mamda/MamdaOrderBookEntry.h \
	mamda/MamdaOrderBookEntryFilter.h \
	mamda/MamdaOrderBookEntryManager.h \
	mamda/MamdaOrderBookExceptions.h \
	mamda/MamdaOrderBookFields.h \
	mamda/MamdaOrderBookGap.h \
	mamda/MamdaOrderBookHandler.h \
	mamda/MamdaOrderBookListener.h \
	mamda/MamdaOrderBookPriceLevel.h \
	mamda/MamdaOrderBookRecap.h \
	mamda/MamdaOrderBookSimpleDelta.h \
	mamda/MamdaOrderBookTypes.h \
	mamda/MamdaQuoteToBookListener.h
	
CPPFLAGS += -I$(srcdir)/.. \
            -I$(srcdir)/../../../../../mama/c_cpp/src/cpp/mama \
            -I$(srcdir)/../../../../../mama/c_cpp/src/cpp \
            -I$(srcdir)/../../../../../mama/c_cpp/src/c \
            -I$(srcdir)/../../../../../common/c_cpp/src/c 

if USE_GCC_FLAGS
CFLAGS += -pedantic -Wall -Wno-long-long
CPPFLAGS += -pedantic -Wall -Wno-long-long -Wno-non-virtual-dtor
endif

libmamdabook_la_SOURCES = \
    MamdaBookAtomicListener.cpp \
	MamdaOrderBook.cpp \
	MamdaOrderBookBasicDelta.cpp \
	MamdaOrderBookBasicDeltaList.cpp \
	MamdaOrderBookChecker.cpp \
	MamdaOrderBookCheckType.cpp \
	MamdaOrderBookConcreteComplexDelta.cpp \
	MamdaOrderBookConcreteSimpleDelta.cpp \
	MamdaOrderBookSimpleDelta.cpp \
	MamdaOrderBookEntry.cpp \
	MamdaOrderBookEntryManager.cpp \
	MamdaOrderBookFields.cpp \
	MamdaOrderBookListener.cpp \
	MamdaOrderBookPriceLevel.cpp \
    MamdaOrderBookWriter.cpp \
	MamdaQuoteToBookListener.cpp
