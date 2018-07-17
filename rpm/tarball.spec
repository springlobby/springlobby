#
# spec file for package SpringLobby
#
# Copyright (c) 2007 tc-
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments via http://trac.springlobby.info/newticket
#
# norootforbuild

#are we not using suse buildservice?
%if ! 0%{?fedora_version}
%if ! 0%{?suse_version}
%if ! 0%{?mandriva_version}
%define noredhat 0
#manual check the distro
%if [ -f /etc/mandrake-release ]
	%define mandriva_version   %(cat /etc/mandrake-release|cut -f4 -d" ")
	%define noredhat 1
%endif
%if [ -f /etc/fedora-release ]
	%define fedora_version      %(cat /etc/fedora-release | cut -f4 -d" ")
	%define noredhat 1
%endif
%if [ -f /etc/redhat-release ]
	%if {noredhat}
		%define redhat_version      %(rpm -qf --queryformat %{VERSION} /etc/redhat-release|tr -d '.')
	%endif
%endif
%if [ -f /etc/SuSE-release ]
	%define suse_version	%(grep VERSION /etc/SuSE-release|cut -f3 -d" ")
%endif

%endif
%endif
%endif


 
Name:           SpringLobby
Version:        1
Release:        1
Url:            https://springlobby.info
License:        GPL
Group:          Amusements/Games/Other
Autoreqprov:    on
Summary:        A cross-platform lobby for the RTS engine Spring
Source:         SpringLobby-%{version}-src.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
 
#define the minimum version number requied for packages
%define req_wxgtk 2.6.0
 
#dependencies check for mandrake-mandriva
%if 0%{?suse_version}
Requires: libwxgtk2.6 >= %{req_wxgtk}
BuildRequires: gcc-c++ , make, libwxgtk2.6-devel >= %{req_wxgtk} 
%endif
 
#dependencies check for SuSe
%if 0%{?suse_version}
Requires: libwxgtk >= %{req_wxgtk}
BuildRequires: gcc-c++ , make, libwxgtk-devel >= %{req_wxgtk} 
%endif
 
#dependencies check for Fedora
%if 0%{?fedora_version}
Requires: libwxgtk >= %{req_wxgtk}
BuildRequires: gcc-c++ , make, libwxgtk-devel >= %{req_wxgtk} 
%endif
 
 
%description
Spring Lobby is a free lobby client for the Spring RTS project. It is implemented in C++, using wxWidgets and is licensed under the GPL. 
 
The client will be designed to match the following design goals which are listed in no particular order: 
Have a very easy to learn and use user interface. 
Support all platforms supported by Spring engine. 
Have all the popular features seen in other clients. 
Have a clean, well designed, and well documented source code.
 
%build
./configure -prefix=/usr
make
 
%install
make install
 
%changelog
* Tue Jul 17 2007 - Brain Damage
- Removed useless deps when building from tarball package
- Added script to auto-detect build envronment when not using SuSe's buildservice

* Sat Jul 14 2007 - Brain Damage
- Initial Version
