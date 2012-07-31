# $Id: Makefile.am,v 1.6.22.9 2011/09/28 16:40:08 emmapollock Exp $
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
## Process this file with automake to produce Makefile.in

ENTITLE_HOME=@entitlements_dir@


install-exec-hook:
if WITH_ENTITLEMENTS
	(cd ${ENTITLE_HOME}/lib/; tar cf - liboeac*) | (cd @prefix@/lib; tar xf -)
	(cd ${ENTITLE_HOME}/bin/; tar cf - oeaclientc) | (cd @prefix@/bin; tar xf -)
endif

SUBDIRS    = src

# Additional installation directory for sample source code
exampledir = @prefix@/examples
