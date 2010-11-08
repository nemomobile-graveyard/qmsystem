%define _name qmsystem2
Name:     libqmsystem2
Version:  0.0.5
Release:  1
Summary:  QmSystem library
Group:    System/Libraries
License:  LGPLv2
URL:      http://meego.gitorious.org/meego-middleware/qmsystem
Source0:  %{_name}-%{version}.tar.bz2
Patch0:   %{name}-0.0.3-proper-pc-file.patch
Patch1:   %{name}-0.0.3-no-devicelock.patch
Patch2:   %{name}-0.0.3-use-pkgconfig.patch

BuildRequires: pkgconfig(dbus-1)
BuildRequires: pkgconfig(dsme)
BuildRequires: pkgconfig(gconf-2.0)
BuildRequires: pkgconfig(libiphb) >= 0.1.4
BuildRequires: pkgconfig(mce) >= 1.10.15
BuildRequires: pkgconfig(QtCore) >= 4.5
BuildRequires: pkgconfig(sensord) >= 0.6.12
#BuildRequires: pkgconfig(timed) >= 1.3.20
BuildRequires: pkgconfig(timed)
BuildRequires: pkgconfig(usb_moded) >= 0.3
BuildRequires: bme-devel >= 0.9.71
#BuildRequires: devicelock-devel >= 0.2.11
BuildRequires: doxygen
BuildRequires: fdupes

Requires: bme >= 0.9.37
Requires: sensorfw >= 0.6.12
Requires: timed >= 1.3.21

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
Requires: testrunner-lite
Requires: %{name} = %{version}-%{release}

%description tests
%{summary}.

%prep
%setup -q -n %{_name}-%{version}
%patch0 -p1
%patch1 -p1
%patch2 -p1

%build
qmake
make

%install
make INSTALL_ROOT=%{buildroot} install
%fdupes %{buildroot}/%{_docdir}/qmsystem2/html/

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%doc debian/changelog debian/copyright
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
%doc debian/copyright
%{_docdir}/qmsystem2/html/*

%files tests
%defattr(-,root,root,-)
%doc debian/copyright
%{_bindir}/*-test
%{_datadir}/qmsystem2-api-fast-tests/*
