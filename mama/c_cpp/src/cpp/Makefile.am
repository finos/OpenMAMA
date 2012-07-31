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

srcdir   = @srcdir@
blddir   = @builddir@
VPATH    = @srcdir@
PACKAGE_VERSION = @PACKAGE_VERSION@

lib_LTLIBRARIES = libmamacpp.la

if WITH_ENTITLEMENTS
# install oea headers
oeadir = $(includedir)/oea
oea_DATA = @entitlements_dir@/include/oea/OeaClient.h \
           @entitlements_dir@/include/oea/OeaSubscription.h \
           @entitlements_dir@/include/oea/OeaStatus.h
endif


nobase_include_HEADERS  = \
	mama/MamaBridgeCallback.h \
    mama/MamaSendCompleteCallback.h \
    mama/MamaBasicSubscription.h \
    mama/MamaBasicWildCardSubscription.h \
    mama/MamaBasicWildCardSubscriptionCallback.h \
    mama/MamaBasicSubscriptionCallback.h \
    mama/MamaDateTime.h \
    mama/MamaDictionary.h \
    mama/MamaDictionaryCallback.h \
    mama/MamaDispatcher.h \
    mama/MamaDQPublisher.h \
    mama/MamaDQPublisherManager.h \
    mama/MamaDQPublisherManagerCallback.h \
    mama/MamaFt.h \
    mama/MamaFieldDescriptor.h \
    mama/MamaInbox.h \
    mama/MamaInboxCallback.h \
    mama/MamaIo.h \
    mama/MamaIoCallback.h \
    mama/MamaMsg.h \
    mama/MamaMsgFieldIterator.h \
    mama/MamaMsgQual.h \
    mama/MamaPublisher.h \
    mama/MamaPrice.h \
    mama/MamaQueue.h \
    mama/MamaQueueEnqueueCallback.h \
    mama/MamaQueueMonitorCallback.h \
    mama/MamaQueueEventCallback.h \
    mama/MamaQueueGroup.h \
    mama/MamaSource.h \
    mama/MamaSourceDerivative.h \
    mama/MamaSourceGroup.h \
    mama/MamaSourceGroupManager.h \
    mama/MamaSourceManager.h \
    mama/MamaSourceStateChangeCallback.h \
    mama/MamaSymbolList.h \
    mama/MamaSymbolListHandlerTypes.h \
    mama/MamaSymbolListMember.h \
    mama/MamaSymbolSource.h \
    mama/MamaSymbolSourceCallback.h \
    mama/MamaSubscription.h \
    mama/MamaSubscriptionCallback.h \
    mama/MamaTimer.h \
    mama/MamaTimerCallback.h \
    mama/MamaTimeZone.h \
    mama/MamaTransport.h \
    mama/MamaTransportMap.h \
    mama/MamaMsgField.h \
    mama/MamaStatus.h \
    mama/MamaSymbolMap.h \
    mama/MamaSymbolMapFile.h \
    mama/MamaLogFile.h \
    mama/conflation/MamaConnection.h \
    mama/mamacpp.h \
    mama/MamaReservedFields.h   


CPPFLAGS += -I@srcdir@ \
            -I@srcdir@/../c \
            -I@builddir@/../c \
            -I${srcdir}/../../../../common/c_cpp/src/c

LDFLAGS +=  -I@srcdir@/../c \
            -I@builddir@/../c \
			-L@builddir@/../c

if USE_GCC_FLAGS
CFLAGS   += -Wmissing-prototypes -Wstrict-prototypes
CPPFLAGS += -pedantic -Wall -Wimplicit -Wno-long-long -D_GNU_SOURCE
endif

libmamacpp_la_SOURCES = \
    datetime.cpp \
    mamacpp.cpp \
    MamaBasicSubscription.cpp \
    MamaBasicWildCardSubscription.cpp \
    MamaDictionary.cpp \
    MamaDispatcher.cpp \
    MamaDQPublisher.cpp \
    MamaDQPublisherManager.cpp \
    MamaFieldDescriptor.cpp \
    MamaFt.cpp \
    MamaInbox.cpp \
    MamaIo.cpp \
    MamaLogFile.cpp \
    MamaMsg.cpp \
    MamaMsgField.cpp \
    MamaMsgQual.cpp \
    MamaPrice.cpp \
    MamaPublisher.cpp \
    MamaQueue.cpp \
    MamaQueueGroup.cpp \
    MamaReservedFields.cpp \
    MamaSource.cpp \
    MamaSourceDerivative.cpp \
    MamaSourceGroup.cpp \
    MamaSourceGroupManager.cpp \
    MamaSourceManager.cpp \
    MamaStatus.cpp \
    MamaSubscription.cpp \
    MamaSymbolList.cpp \
    MamaSymbolListMember.cpp \
    MamaSymbolMapFile.cpp \
    MamaTimeZone.cpp \
    MamaTimer.cpp \
    MamaTransport.cpp \
    MamaTransportMap.cpp \
    conflation/MamaConnection.cpp \
    conflation/MamaServerConnection.cpp
