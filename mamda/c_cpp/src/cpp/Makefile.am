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

SUBDIRS = . news options orderbooks 

lib_LTLIBRARIES = libmamda.la

nobase_include_HEADERS  = \
    mamda/MamdaVersion.h \
    mamda/MamdaLock.h \
    mamda/MamdaBasicEvent.h \
    mamda/MamdaBasicRecap.h \
    mamda/MamdaBasicSubscription.h \
    mamda/MamdaCommonFields.h \
    mamda/MamdaConcreteBasicEvent.h \
    mamda/MamdaDataException.h \
    mamda/MamdaErrorListener.h \
    mamda/MamdaFields.h \
    mamda/MamdaFieldState.h \
    mamda/MamdaFundamentalFields.h \
    mamda/MamdaFundamentalHandler.h \
    mamda/MamdaFundamentalListener.h \
    mamda/MamdaFundamentals.h \
    mamda/MamdaMsgListener.h \
    mamda/MamdaMultiParticipantHandler.h \
    mamda/MamdaMultiParticipantManager.h \
    mamda/MamdaMultiSecurityHandler.h \
    mamda/MamdaMultiSecurityManager.h \
    mamda/MamdaOrderImbalanceRecap.h \
    mamda/MamdaOrderImbalanceUpdate.h \
    mamda/MamdaOrderImbalanceHandler.h \
    mamda/MamdaPubStatusFields.h \
    mamda/MamdaPubStatus.h \
    mamda/MamdaPubStatusHandler.h \
    mamda/MamdaPubStatusListener.h \
    mamda/MamdaQualityListener.h \
    mamda/MamdaQuoteClosing.h \
    mamda/MamdaQuoteFields.h \
    mamda/MamdaQuoteGap.h \
    mamda/MamdaQuoteHandler.h \
    mamda/MamdaQuoteListener.h \
    mamda/MamdaQuoteChecker.h \
    mamda/MamdaCheckerHandler.h \
    mamda/MamdaCheckerType.h \
    mamda/MamdaQuoteOutOfSequence.h \
    mamda/MamdaQuotePossiblyDuplicate.h \
    mamda/MamdaQuoteRecap.h \
    mamda/MamdaQuoteUpdate.h \
    mamda/MamdaSecStatusFields.h \
    mamda/MamdaSecStatusHandler.h \
    mamda/MamdaSecStatusListener.h \
    mamda/MamdaSecStatus.h \
    mamda/MamdaSecStatusRecap.h \
    mamda/MamdaSecStatusSymbolSourceAdapter.h \
    mamda/MamdaSecurityStatus.h \
    mamda/MamdaSecurityStatusQual.h \
    mamda/MamdaSubscription.h \
    mamda/MamdaTradeCancelOrError.h \
    mamda/MamdaTradeClosing.h \
    mamda/MamdaTradeCorrection.h \
    mamda/MamdaTradeDirection.h \
    mamda/MamdaTradeExecVenue.h \
    mamda/MamdaTradeSide.h \
    mamda/MamdaTradeFields.h \
    mamda/MamdaTradeGap.h \
    mamda/MamdaTradeHandler.h \
    mamda/MamdaTradeListener.h \
    mamda/MamdaTradeOutOfSequence.h \
    mamda/MamdaTradePossiblyDuplicate.h \
    mamda/MamdaTradeRecap.h \
    mamda/MamdaTradeReport.h \
    mamda/MamdaTradeChecker.h \
    mamda/MamdaOrderImbalanceFields.h \
    mamda/MamdaOrderImbalanceType.h \
    mamda/MamdaOrderImbalanceSide.h \
    mamda/MamdaOrderImbalanceListener.h \
    mamda/MamdaCurrencyFields.h \
    mamda/MamdaCurrencyUpdate.h \
    mamda/MamdaCurrencyRecap.h \
    mamda/MamdaCurrencyListener.h \
    mamda/MamdaCurrencyHandler.h \
    mamda/MamdaConfig.h \
    mamda/MamdaOptionalConfig.h \
    mamda/MamdaQuery.h \
    mamda/MamdaUncrossPriceInd.h \
    mamda/MamdaAuctionListener.h \
    mamda/MamdaAuctionFields.h \
    mamda/MamdaAuctionHandler.h \
    mamda/MamdaAuctionRecap.h \
    mamda/MamdaAuctionUpdate.h 


CPPFLAGS += -I$(srcdir)/.. \
            -I$(srcdir)/../../../../mama/c_cpp/src/cpp/mama \
            -I$(srcdir)/../../../../mama/c_cpp/src/cpp \
            -I$(srcdir)/../../../../mama/c_cpp/src/c \
            -I$(srcdir)/../../../../common/c_cpp/src/c 

if USE_GCC_FLAGS
CPPFLAGS += -pedantic -Wall -Wno-long-long -Wno-non-virtual-dtor -D_GNU_SOURCE
endif

libmamda_la_SOURCES = \
    MamdaLock.cpp \
    MamdaBasicSubscription.cpp \
    MamdaCommonFields.cpp \
    MamdaConcreteBasicEvent.cpp \
    MamdaFundamentalFields.cpp \
    MamdaFundamentalListener.cpp \
    MamdaMultiParticipantManager.cpp \
    MamdaMultiSecurityManager.cpp \
    MamdaPubStatusFields.cpp \
    MamdaPubStatusListener.cpp \
    MamdaQuoteFields.cpp \
    MamdaQuoteListener.cpp \
    MamdaQuoteChecker.cpp \
    MamdaCheckerType.cpp \
    MamdaSecStatusSymbolSourceAdapter.cpp \
    MamdaSecurityStatus.cpp \
    MamdaSecurityStatusQual.cpp \
    MamdaSecStatusFields.cpp \
    MamdaSecStatusListener.cpp \
    MamdaSubscription.cpp \
    MamdaTradeDirection.cpp \
    MamdaTradeExecVenue.cpp \
    MamdaTradeSide.cpp\
    MamdaTradeFields.cpp \
    MamdaTradeListener.cpp \
    MamdaTradeChecker.cpp \
    MamdaUtils.cpp \
    MamdaVersion.cpp \
    MamdaOrderImbalanceFields.cpp \
    MamdaOrderImbalanceType.cpp \
    MamdaOrderImbalanceSide.cpp \
    MamdaOrderImbalanceListener.cpp \
    MamdaCurrencyFields.cpp \
    MamdaCurrencyListener.cpp \
    MamdaQuery.cpp \
    MamdaUncrossPriceInd.cpp \
    MamdaAuctionFields.cpp \
    MamdaAuctionListener.cpp


nodist_libmamda_la_SOURCES = \
	@builddir@/version.c

version.c:
	echo "const char* mamda_version = \"$(PACKAGE_NAME) $(PACKAGE_VERSION)\";" > version.c
