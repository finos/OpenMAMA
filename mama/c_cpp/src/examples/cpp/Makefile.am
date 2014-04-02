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

mama_platform = @mama_platform@
srcdir        = @srcdir@
VPATH         = @srcdir@


CPPFLAGS   += -I$(srcdir)/../../c \
              -I$(srcdir)/../../cpp \
              -I${srcdir}/../../../../../common/c_cpp/src/c \
              -I${srcdir}/../../../../../common/c_cpp/src/cpp


if USE_GCC_FLAGS
CFLAGS   += -std=gnu99 -pedantic-errors -Wmissing-prototypes -Wstrict-prototypes
CPPFLAGS += -pedantic-errors -Wall -Wimplicit -Wno-long-long -D_GNU_SOURCE
endif

LDFLAGS  += -L${srcdir}/../../cpp \
            -L${srcdir}/../../c \
            -L${srcdir}/../../../../../common/c_cpp/src/c


# Link impl libs twice to get round static linking issues  
LDADD     =  -lmamacpp -lmama -lpthread -ldl
#LDADD.cpp =  -L../c -L../cpp -L../c/bridge/$(mama_platform) -lmamacpp $(LDADD) 


bin_PROGRAMS =  mamalistencpp   \
                mamasubscribercpp \
                mamaproxycpp \
                mamapublishercpp \
                mamainboxcpp \
                mamaiocpp \
                mamaftmembercpp \
				mamasymbollistsubscribercpp \
				mamalistencachedcpp

dist_mamalistencpp_SOURCES              	= mamalistencpp.cpp     
dist_mamaproxycpp_SOURCES            		= mamaproxycpp.cpp
dist_mamapublishercpp_SOURCES           	= mamapublishercpp.cpp 
dist_mamasubscribercpp_SOURCES           	= mamasubscribercpp.cpp
dist_mamainboxcpp_SOURCES                	= mamainboxcpp.cpp
dist_mamaiocpp_SOURCES                    	= mamaiocpp.cpp
dist_mamasymbollistsubscribercpp_SOURCES 	= mamasymbollistsubscribercpp.cpp
dist_mamaftmembercpp_SOURCES 	    		= mamaftmembercpp.cpp
dist_mamalistencachedcpp_SOURCES 			= mamalistencachedcpp.cpp

# Additional installation directory for sample source code
exampledir = ${prefix}/examples/mama/cpp

# Rule to install the example programs:
example_DATA = \
        ../mama.properties \
        $(dist_mamalistencpp_SOURCES) \
        $(dist_mamaproxycpp_SOURCES) \
        $(dist_mamaftmembercpp_SOURCES) \
        $(dist_mamapublishercpp_SOURCES) \
        $(dist_mamasubscribercpp_SOURCES) \
        $(dist_mamainboxcpp_SOURCES) \
        $(dist_mamaiocpp_SOURCES) \
		$(dist_mamasymbollistsubscribercpp_SOURCES) \
		$(dist_mamalistencachedcpp_SOURCES) \
		Makefile.sample
