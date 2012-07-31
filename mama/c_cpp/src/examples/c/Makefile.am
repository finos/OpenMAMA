# $Id: Makefile.am,v 1.1.2.7 2011/09/27 15:45:03 emmapollock Exp $
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


CFLAGS   += -I$(srcdir)/../../c \
            -I${srcdir}/../../../../../common/c_cpp/src/c

if USE_GCC_FLAGS
CFLAGS   += -Wmissing-prototypes -Wstrict-prototypes
CPPFLAGS += -pedantic -Wall -Wimplicit -Wno-long-long -D_GNU_SOURCE
endif

LDFLAGS  += -L${srcdir}/../../c \
            -L${srcdir}/../../../../../common/c_cpp/src/c


# Link impl libs twice to get round static linking issues  
LDADD     =  -lmama -lpthread -lm -lwombatcommon -lrt
LDADD.cpp =  -L../c -L../cpp -L../c/bridge/$(mama_platform) -lmamacpp $(LDADD) 


bin_PROGRAMS =  mamalistenc \
                mamasubscriberc \
                mamaproxyc \
                mamapublisherc \
                mamainboxc \
                mamaio \
                mamamultisubscriberc \
                mamaftmemberc \
				mamasymbollistsubscriberc 

dist_mamalistenc_SOURCES            	= mamalistenc.c     
dist_mamaproxyc_SOURCES          		= mamaproxyc.c
dist_mamapublisherc_SOURCES         	= mamapublisherc.c 
dist_mamasubscriberc_SOURCES        	= mamasubscriberc.c
dist_mamainboxc_SOURCES             	= mamainboxc.c
dist_mamaio_SOURCES                 	= mamaio.c
dist_mamasymbollistsubscriberc_SOURCES 	= mamasymbollistsubscriberc.c
dist_mamamultisubscriberc_SOURCES 	    = mamamultisubscriberc.c
dist_mamaftmemberc_SOURCES 	    		= mamaftmemberc.c

# Additional installation directory for sample source code
exampledir = ${prefix}/examples/mama/c

# Rule to install the example programs:
example_DATA = \
        ../mama.properties \
        $(dist_mamalistenc_SOURCES) \
        $(dist_mamaproxyc_SOURCES) \
        $(dist_mamamultisubscriberc_SOURCES) \
        $(dist_mamaftmemberc_SOURCES) \
        $(dist_mamapublisherc_SOURCES) \
        $(dist_mamasubscriberc_SOURCES) \
        $(dist_mamainboxc_SOURCES) \
        $(dist_mamaio_SOURCES) \
		$(dist_mamasymbollistsubscriberc_SOURCES) \
  		Makefile.sample
