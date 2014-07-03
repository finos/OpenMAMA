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
blddir=@builddir@
VPATH  = @srcdir@
PACKAGE_VERSION = @PACKAGE_VERSION@

SUBDIRS = . bridge payload

lib_LTLIBRARIES = libmama.la             

if WITH_ENTITLEMENTS
# install oea headers
oeadir = $(includedir)/oea
oea_DATA = @entitlements_dir@/include/oea/OeaClient.h \
           @entitlements_dir@/include/oea/OeaSubscription.h \
           @entitlements_dir@/include/oea/OeaStatus.h
endif

                            
nobase_include_HEADERS  = \
	mama/clientmanage.h \
	mama/config.h \
	mama/datetime.h \
	mama/dictionary.h \
    mama/error.h \
	mama/fielddesc.h \
	mama/inbox.h \
	mama/io.h \
	mama/log.h \
	mama/mama.h \
	mama/marketdata.h \
    mama/middleware.h \
	mama/msg.h \
	mama/msgfield.h \
	mama/msgqualifier.h \
	mama/msgstatus.h \
	mama/msgtype.h \
	mama/price.h \
	mama/publisher.h \
    mama/dqpublisher.h \
    mama/dqpublishermanager.h \
	mama/quality.h \
	mama/queue.h \
	mama/reservedfields.h \
	mama/senderId.h \
	mama/servicelevel.h \
	mama/source.h \
	mama/sourceman.h \
	mama/stat.h \
	mama/statscollector.h \
	mama/statfields.h \
	mama/status.h \
	mama/subscmsgtype.h \
	mama/subscription.h \
	mama/subscriptiontype.h \
	mama/symbollist.h \
	mama/ft.h \
	mama/symbollistmember.h \
	mama/symbollisttypes.h \
    mama/symbolmap.h \
	mama/symbolmapfile.h \
	mama/timer.h \
	mama/timezone.h \
	mama/transport.h \
	mama/types.h \
    mama/conflation/connection.h \
	mama/version.h \
    mama/fieldcache/fieldcache.h \
    mama/fieldcache/fieldcachefield.h \
    mama/fieldcache/fieldcacheiterator.h \
    mama/fieldcache/fieldcacherecord.h \
    mama/fieldcache/fieldcachetypes.h

   
CFLAGS   += -I${srcdir}/../../../../common/c_cpp/src/c
LDFLAGS  += -L${srcdir}/../../../../common/c_cpp/src/c

LIBS     += -lm -lrt -lwombatcommon -ldl -lpthread


if USE_GCC_FLAGS
CFLAGS   += -std=gnu99 -pedantic-errors -Wmissing-prototypes -Wstrict-prototypes
CPPFLAGS += -pedantic-errors -Wall -Wno-long-long -D_GNU_SOURCE
endif


if DEV_RELEASE
CPPFLAGS += -DDEV_RELEASE
endif


if WITH_ENTITLEMENTS
CPPFLAGS += -DWITH_ENTITLEMENTS
LIBS += -loeac
endif


libmama_la_SOURCES = \
    bridge.c \
	clientmanageresponder.c \
    datetime.c \
	dictionary.c \
	dqstrategy.c \
    error.c \
	fielddesc.c \
	imagerequest.c \
	listenermsgcallback.c \
	log.c \
	mamainternal.h \
    mama.c \
    mamaStrUtils.h \
    mamaStrUtils.c \
	marketdata.c \
    middleware.c \
	msg.c \
	msgfield.c \
	msgqualifier.c \
	msgstatus.c \
	msgtype.c \
	msgutils.c \
    publisher.c \
    dqpublisher.c \
    dqpublishermanager.c \
    inbox.c \
	quality.c \
	reservedfields.c \
	senderId.c \
	servicelevel.c \
    queue.c \
    io.c    \
    price.c \
    priceimpl.c \
    refreshtransport.c \
    timer.c \
    timezone.c \
	source.c \
	sourceman.c \
    status.c \
	stat.c \
	statscollector.c \
	statfields.c \
	statsgenerator.c \
	subscmsgtype.c \
	subscription.c \
	subscriptiontype.c \
	statslogger.c \
	statsloggerfields.c \
	symbollist.c \
	ft.c \
	symbollistmember.c \
	symbolmapfile.c \
	syncresponder.c \
	throttle.c \
	transport.c \
    fileutils.c \
    playback/playbackFileParser.c \
    playback/playbackpublisher.c \
    playback/playbackcapture.c \
    conflation/connection.c \
    conflation/serverconnection.c \
    conflation/manager.c \
    fieldcache/fieldcachevector.c \
    fieldcache/fieldcachelist.c \
    fieldcache/fieldcachemap.c \
    fieldcache/fieldcachemaparray.c \
    fieldcache/fieldcachemapbinary.c \
    fieldcache/fieldcachemapmonitor.c \
    fieldcache/fieldcacheiterator.c \
    fieldcache/fieldcachefieldimpl.c \
    fieldcache/fieldcachefield.c \
    fieldcache/fieldcacherecord.c \
    fieldcache/fieldcacheimpl.c \
    fieldcache/fieldcache.c
        
nodist_libmama_la_SOURCES = \
	@builddir@/version.c

version.c:
	echo "const char* mama_version = \"openmama $(PACKAGE_VERSION)\";" > version.c
