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
%define req_automake 1.8
%define req_autoconf 2.59

#dependencies check for mandrake-mandriva
%if 0%{?suse_version}
Requires: libwxgtk2.6 >= %{req_wxgtk}
BuildRequires: gcc-c++ , make, subversion, automake1.8 >= %{req_automake}, autoconf2.5 >= %{req_autoconf}, libwxgtk2.6-devel >= %{req_wxgtk} 
%endif

#dependencies check for SuSe
%if 0%{?suse_version}
Requires: libwxgtk >= %{req_wxgtk}
BuildRequires: gcc-c++ , make, subversion, automake >= %{req_automake}, autoconf >= %{req_autoconf}, libwxgtk-devel >= %{req_wxgtk} 
%endif

#dependencies check for Fedora
%if 0%{?fedora_version}
Requires: libwxgtk >= %{req_wxgtk}
BuildRequires: gcc-c++ , make, subversion, automake >= %{req_automake}, autoconf >= %{req_autoconf}, libwxgtk-devel >= %{req_wxgtk} 
%endif


%description
Spring Lobby is a free lobby client for the Spring RTS project. It is implemented in C++, using wxWidgets and is licensed under the GPL. 

The client will be designed to match the following design goals which are listed in no particular order: 
Have a very easy to learn and use user interface. 
Support all platforms supported by Spring engine. 
Have all the popular features seen in other clients. 
Have a clean, well designed, and well documented source code.

%prep
svn co http://svn.springlobby.info/trunk springlobby && cd springlobby
./autogen.sh

%build
./configure
make

%install
make install

%changelog
* Sat Jul 14 2007 - Brain Damage
- Initial Version
