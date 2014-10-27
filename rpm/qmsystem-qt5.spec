Name:       qmsystem-qt5
Summary:    QmSystem library
Version:    1.4.19
Release:    1
Group:      System/System Control
License:    LGPLv2
URL:        http://github.com/nemomobile/qmsystem
Source0:    %{name}-%{version}.tar.bz2
Source100:  qmsystem-qt5.yaml
Requires:   sensorfw-qt5 >= 0.6.33
Requires:   timed-qt5 >= 2.31
Requires:   mce
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(mce)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(dsme)
BuildRequires:  pkgconfig(dsme_dbus_if)
BuildRequires:  pkgconfig(libiphb) >= 0.61.29
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Network)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(sensord-qt5) >= 0.6.33
BuildRequires:  pkgconfig(timed-qt5) >= 2.31
BuildRequires:  fdupes

%description
This package contains the QmSystem library.

%package tests
Summary:    Unit test cases and xml test description for libqmsystem2 library
Group:      Development/System
Requires:   %{name} = %{version}-%{release}
Requires:   testrunner-lite

%description tests
%{summary}.

%package devel
Summary:    Development headers for QmSystem library
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Development headers for compiling applications against the QmSystem library.

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5
make

%install
rm -rf %{buildroot}
%make_install
%fdupes %{buildroot}/%{_docdir}/qmsystem2-qt5/html/

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%doc debian/copyright
%{_libdir}/libqmsystem2-qt5.so.*
%{_sbindir}/qmkeyd2-qt5

%files tests
%defattr(-,root,root,-)
%doc debian/copyright
/opt/tests/qmsystem-qt5-tests/*-test
%{_datadir}/%{name}-tests/tests.xml

%files devel
%defattr(-,root,root,-)
%doc debian/copyright
%{_includedir}/qmsystem2-qt5/*.h
%{_libdir}/libqmsystem2-qt5.so
%{_libdir}/pkgconfig/qmsystem2-qt5.pc
%{_datadir}/qt5/mkspecs/features/qmsystem2-qt5.prf

