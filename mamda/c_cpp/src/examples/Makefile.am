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

SUBDIRS = . news orderbooks 


CPPFLAGS += -I$(srcdir)/../cpp \
            -I$(srcdir)/../cpp/mamda \
            -I$(srcdir)/../cpp/options \
            -I$(srcdir)/../../../../mama/c_cpp/src/c \
            -I$(srcdir)/../../../../mama/c_cpp/src/cpp \
            -I$(srcdir)/../../../../common/c_cpp/src/c \
            -I/usr/local/include

LDFLAGS  += -L${srcdir}/../cpp \
            -L$(srcdir)/../cpp/mamda \
            -L$(srcdir)/../cpp/options \
            -L$(srcdir)/../../../../mama/c_cpp/src/c \
            -L$(srcdir)/../../../../mama/c_cpp/src/cpp

if USE_GCC_FLAGS
  LDFLAGS += -rdynamic
endif

LIBS += -lmamda -lmamacpp -lmama -lpthread -ldl -lnsl -lm

optionchainer_LDADD      = -lmamdaoptions
optionview_LDADD         = -lmamdaoptions

bin_PROGRAMS = comboticker \
               mamdalisten \
               multipartticker \
               multisecurityticker \
               optionchainer \
               optionview \
               quoteticker \
               tradeticker \
               orderimbalanceticker\
               secstatuslisten \
               fundamentallisten \
			   currencyticker \
               auctionticker

common_files = parsecmd.cpp dictrequester.cpp

dist_comboticker_SOURCES          = comboticker.cpp         $(common_files)
dist_mamdalisten_SOURCES          = mamdalisten.cpp         $(common_files)
dist_multipartticker_SOURCES      = multipartticker.cpp     $(common_files)
dist_multisecurityticker_SOURCES  = multisecurityticker.cpp $(common_files)
dist_optionchainer_SOURCES        = optionchainer.cpp       $(common_files)
dist_optionview_SOURCES           = optionview.cpp          $(common_files)
dist_quoteticker_SOURCES          = quoteticker.cpp         $(common_files)
dist_tradeticker_SOURCES          = tradeticker.cpp         $(common_files)
dist_orderimbalanceticker_SOURCES = orderimbalanceticker.cpp $(common_files)
dist_secstatuslisten_SOURCES      = secstatuslisten.cpp     $(common_files)
dist_fundamentallisten_SOURCES    = fundamentallisten.cpp   $(common_files)
dist_currencyticker_SOURCES       = currencyticker.cpp       $(common_files)
dist_auctionticker_SOURCES        = auctionticker.cpp       $(common_files)


# Additional installation directory for sample source code
exampledir = ${prefix}/examples/mamda/cpp

# Rule to install the example programs:
example_DATA = \
    Makefile.sample \
    parsecmd.h \
	parsecmd.cpp \
    dictrequester.h \
	dictrequester.cpp \
    comboticker.cpp \
    quoteticker.cpp \
    optionchainer.cpp  \
    optionview.cpp \
    tradeticker.cpp \
    mamdalisten.cpp \
    multipartticker.cpp \
    multisecurityticker.cpp \
    secstatuslisten.cpp \
    fundamentallisten.cpp \
    currencyticker.cpp \
    orderimbalanceticker.cpp \
    auctionticker.cpp
