Summary: An abstraction layer which sits on top of multiple message oriented middlewares
Name:       openmama
Version:    %{?BUILD_VERSION}
Release:    %{?BUILD_NUMBER}%{?dist}
License:    LGPLv2
URL:        http://www.openmama.org
Group:      Development/Libraries
Source:     %{name}-%{version}.tgz
BuildRoot:  %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: libtool autoconf automake ant libuuid-devel flex doxygen qpid-proton-c-devel scons libevent-devel ncurses-devel
Requires: libuuid qpid-proton-c libevent ncurses

%if 0%{?fedora}
BuildRequires: java-1.8.0-openjdk-devel
Requires: java-1.8.0-openjdk
%define java_home /usr/lib/jvm/java/
%endif

%if 0%{?rhel}
BuildRequires: java
Requires: java
%define java_home /usr/lib/jvm/java/
%endif

%description
OpenMAMA is a high performance abstraction layer for message oriented
middlewares, where MAMA stands for Middleware Agnostic Messaging API.

%prep
%setup -q

%build
scons middleware=qpid product=mamdajni java_home=%{java_home} with_testtools=yes jobs=n
# prefix=%{buildroot}/opt/openmama/

%install
# scons install
rm -rf %{buildroot}
mkdir -p %{buildroot}/opt/openmama/
mkdir %{buildroot}/opt/openmama/data
mkdir %{buildroot}/opt/openmama/config
cp -r %{_builddir}/%{name}-%{version}/data/* %{buildroot}/opt/openmama/data/
cp -r %{_builddir}/%{name}-%{version}/openmama_install_*/* %{buildroot}/opt/openmama
cp %{_builddir}/%{name}-%{version}/data/profiles/profile.openmama %{buildroot}/opt/openmama/config/
cp %{_builddir}/%{name}-%{version}/*.md %{buildroot}/opt/openmama/
cp %{_builddir}/%{name}-%{version}/mama/c_cpp/src/examples/mama.properties %{buildroot}/opt/openmama/config

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
/opt/openmama/bin/*
/opt/openmama/lib/*
/opt/openmama/examples/*
/opt/openmama/config/*
/opt/openmama/data/*
/opt/openmama/include/destroyHandle.h
/opt/openmama/include/list.h
/opt/openmama/include/lookup2.h
/opt/openmama/include/platform.h
/opt/openmama/include/timers.h
/opt/openmama/include/property.h
/opt/openmama/include/wlock.h
/opt/openmama/include/wombat
/opt/openmama/include/mama
/opt/openmama/include/mamda
/opt/openmama/README
/opt/openmama/COPYING

%changelog
