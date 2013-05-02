# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.25
# 

Name:       qmsystem-qt5

# >> macros
# << macros

Summary:    QmSystem library
Version:    1.4.6
Release:    1
Group:      System/System Control
License:    LGPLv2
URL:        http://github.com/nemomobile/qmsystem
Source0:    %{name}-%{version}.tar.bz2
Requires:   sensorfw-qt5 >= 0.6.33
Requires:   timed-qt5 >= 2.31
Requires:   mce
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(mce)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(dsme)
BuildRequires:  pkgconfig(dsme_dbus_if)
BuildRequires:  pkgconfig(gconf-2.0)
BuildRequires:  pkgconfig(libiphb) >= 0.61.29
BuildRequires:  pkgconfig(Qt5Core) >= 4.5
BuildRequires:  pkgconfig(sensord-qt5) >= 0.6.33
BuildRequires:  pkgconfig(timed-qt5) >= 2.31
BuildRequires:  fdupes

%description
This package contains the QmSystem library.

%package devel
Summary:    Development headers for QmSystem library
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Development headers for compiling applications against the QmSystem library.


%prep
%setup -q -n %{name}-%{version}

# >> setup
# << setup

%build
# >> build pre
%qmake5
make
# << build pre



# >> build post
# << build post

%install
rm -rf %{buildroot}
# >> install pre
# << install pre

# >> install post
%make_install
%fdupes %{buildroot}/%{_docdir}/qmsystem2/html/
# << install post


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
# >> files
%doc debian/copyright
%{_libdir}/libqmsystem2-qt5.so.*
%exclude %{_sbindir}/qmkeyd2
%exclude %{_bindir}/*-test
%exclude %{_datadir}/%{name}-tests/tests.xml
# From doc
# %doc debian/changelog debian/copyright
# %{_docdir}/qmsystem2/html/*
# << files

%files devel
%defattr(-,root,root,-)
# >> files devel
%doc debian/copyright
%{_includedir}/qmsystem2-qt5/*.h
%{_libdir}/libqmsystem2-qt5.so
%{_libdir}/pkgconfig/qmsystem2-qt5.pc
%{_datadir}/qt4/mkspecs/features/qmsystem2-qt5.prf
# << files devel
