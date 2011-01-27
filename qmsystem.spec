Name:     qmsystem
Version:  1.0.6_1
Release:  1
Summary:  QmSystem library
Group:    System/System Control
License:  LGPLv2
URL:      http://meego.gitorious.org/meego-middleware/qmsystem
Source0:  %{name}-%{version}.tar.bz2
Patch0:   %{name}-1.0.0-tests-directories.patch

BuildRequires: pkgconfig(dbus-1)
BuildRequires: pkgconfig(dsme)
BuildRequires: pkgconfig(dsme_dbus_if)
BuildRequires: pkgconfig(gconf-2.0)
BuildRequires: pkgconfig(libiphb) >= 0.1.4
BuildRequires: pkgconfig(mce) >= 1.10.15
BuildRequires: pkgconfig(QtCore) >= 4.5
BuildRequires: pkgconfig(sensord) >= 0.6.33
#BuildRequires: pkgconfig(sysinfo) >= 1.0.17
BuildRequires: pkgconfig(timed) >= 2.31
#BuildRequires: pkgconfig(usb_moded) >= 0.24
#BuildRequires: devicelock-devel >= 0.2.11
BuildRequires: doxygen
BuildRequires: fdupes
BuildRequires: graphviz

Requires: sensorfw >= 0.6.33
Requires: timed >= 2.31

%description
This package contains the QmSystem library.

%package devel
Summary:  Development headers for QmSystem library
Group:    Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
Development headers for compiling applications against the QmSystem library.

%package doc
Summary:  API documentation for libqmsystem2
Group:    Documentation

%description doc
Doxygen-generated API documentation for QmSystem library.

%package tests
Summary:  Unit test cases and xml test description for libqmsystem2 library
Group:    Development/System
Requires: %{name} = %{version}-%{release}
Requires: testrunner-lite

%description tests
%{summary}.

%prep
%setup -q
%patch0 -p1

%build
%qmake
make

%install
%qmake_install
%fdupes %{buildroot}/%{_docdir}/qmsystem2/html/

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%doc debian/copyright
%{_libdir}/libqmsystem2.so.*
%{_sbindir}/qmkeyd2

%files devel
%defattr(-,root,root,-)
%doc debian/copyright
%{_includedir}/qmsystem2/*.h
%{_libdir}/libqmsystem2.so
%{_libdir}/pkgconfig/qmsystem2.pc
%{_datadir}/qt4/mkspecs/features/qmsystem2.prf

%files doc
%defattr(-,root,root,-)
%doc debian/changelog debian/copyright
%{_docdir}/qmsystem2/html/*

%files tests
%defattr(-,root,root,-)
%doc debian/copyright
%{_bindir}/*-test
%{_datadir}/%{name}-tests/tests.xml
