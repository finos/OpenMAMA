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

# Pick up the install directory as set by configure
PREFIX :=$(shell cat prefix.txt)

.PHONY: all docs mama mama-install mama-clean mamajni mamajni-install mamajni-clean\
		mamda mamda-install mamda-clean mamdajni mamdajni-install mamdajni-clean\
		common common-install common-clean test

all: common mama mamajni mamda mamdajni docs

install: common-install mama-install mamajni-install mamda-install mamdajni-install

clean: common-clean mama-clean mamajni-clean mamda-clean mamdajni-clean


#####################################################
# COMMON
#####################################################
common: 
	cd common/c_cpp ; make ; cd -

common-install:
	cd common/c_cpp ; make install -j1 -l80 ; cd -

common-clean:
	cd common/c_cpp ; make clean -j1 -l80 ; cd -


#####################################################
# MAMA
#####################################################
mama: common
	cd mama/c_cpp ; make ; cd -

mama-install: common-install
	cd mama/c_cpp ; make install -j1 -l80 ; cd -

mama-clean:
	cd mama/c_cpp ; make clean; cd -


#####################################################
# MAMAJNI
#####################################################
mamajni: mama
	cd mama/jni ; ant -f build.xml ; make ; cd -

mamajni-install: mama-install mamajni
	cd mama/jni ; ant -f build.xml ; make install -j1 -l80 ; cd -
	$(eval MAMAJNI_BUILD := mama/jni/$(shell ls -1 mama/jni | grep mamajni))
	cp $(MAMAJNI_BUILD)/lib/*.jar $(PREFIX)/lib
	mkdir -p $(PREFIX)/examples/mamajni
	cp $(MAMAJNI_BUILD)/examples/*.java $(PREFIX)/examples/mamajni

mamajni-clean: mama-clean
	cd mama/jni ; ant -f build.xml clean ; make clean ; cd -
	$(eval MAMAJNI_BUILD := mama/jni/$(shell ls -1 mama/jni | grep mamajni))
	rm -r $(MAMAJNI_BUILD)


#####################################################
# MAMDA
#####################################################
mamda: mama
	cd mamda/c_cpp ; make ; cd -

mamda-install: mama-install
	cd mamda/c_cpp ; make install -j1 -l80 ; cd -

mamda-clean: mama-clean
	cd mamda/c_cpp ; make clean; cd -


#####################################################
# MAMDAJAVA
#####################################################
mamdajni: mamajni
	cd mamda/java ; ant -f build.xml dist; cd -

mamdajni-install: mamajni-install
	cd mamda/java ; ant -f build.xml dist ; cd -
	$(eval MAMDAJNI_BUILD := mamda/java/mamda)
	cp $(MAMDAJNI_BUILD)/bin/* $(PREFIX)/bin
	cp $(MAMDAJNI_BUILD)/lib/*.jar $(PREFIX)/lib
	mkdir -p $(PREFIX)/examples/mamdajni
	cp -r $(MAMDAJNI_BUILD)/examples/com/wombat/mamda/examples/*.java $(PREFIX)/examples/mamdajni

mamdajni-clean: mamajni-clean
	cd mamda/java ; ant -f build.xml clean ; cd -
	$(eval MAMDAJNI_BUILD := mamda/java/mamda)
	rm -r $(MAMDAJNI_BUILD)


#####################################################
# DOCS
#####################################################
docs:
	echo "Building API Docs in mama/c_cpp/docs"
	cd mama/c_cpp ; doxygen doxyconfig-c ; cd -
	cd mama/c_cpp ; doxygen doxyconfig-cpp ; cd -
