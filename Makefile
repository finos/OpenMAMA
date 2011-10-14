# $Id: Makefile,v 1.1.2.4 2011/09/23 13:07:23 emmapollock Exp $
#
# OpenMAMA: The open middleware agnostic messaging API
# Copyright (C) 2011 NYSE Inc.
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

.PHONY: all docs mama mama-install mama-clean \
        common common-install common-clean

all: common mama 

docs:
	echo "Building API Docs in mama/c_cpp/docs"
	cd mama/c_cpp ; doxygen doxyconfig-c ; cd -

mama: common
	cd mama/c_cpp ; make ; cd -

mama-install: common-install
	cd mama/c_cpp ; make install -j1 -l80 ; cd -

mama-clean:
	cd mama/c_cpp ; make clean; cd -

common: 
	cd common/c_cpp ; make ; cd -

common-install:
	cd common/c_cpp ; make install -j1 -l80 ; cd -

common-clean:
	cd common/c_cpp ; make clean -j1 -l80 ; cd -

install: common-install mama-install

clean: common-clean mama-clean
