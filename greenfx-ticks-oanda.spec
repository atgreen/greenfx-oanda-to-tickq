Name:		greenfx-ticks-oanda
Version:	1.0
Release:	0.1
Summary:	Collect and publish ticks from Oanda

Group:	        Applications
License:	GPL
URL:		http://github.com/atgreen/greenfx-ticks-oanda
Source0:	main.cc
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:  libcurl-devel activemq-cpp-devel json-c-devel

%description
Collect and public ticks from Oanda.

%prep
cp %{SOURCE0} .

%build
g++ -O3 -g -o ticks-oanda main.cc `pkg-config activemq-cpp --cflags` -lcurl `pkg-config activemq-cpp --libs` -ljson-c

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{_bindir}
cp ticks-oanda $RPM_BUILD_ROOT%{_bindir}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_bindir}/*

%changelog
* Thu Sep 29 2016 Anthony Green <green@moxielogic.com> 1.0-1
- Created.
