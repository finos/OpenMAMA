# $Id$

srcdir = @srcdir@
blddir = @builddir@
VPATH  = @srcdir@
PACKAGE_VERSION = @PACKAGE_VERSION@

SUBDIRS = . 

lib_LTLIBRARIES = libwombatcommoncpp.la             

nobase_include_HEADERS  = \
	wombat/Lock.h

LIBS     += -ldl -lrt -lwombatcommon

if USE_GCC_FLAGS
CPPFLAGS += -pedantic -fpic -Wall -Wno-long-long -D_GNU_SOURCE
endif

CPPFLAGS   += -I@srcdir@/../c

libwombatcommoncpp_la_SOURCES = \
	Lock.cpp

