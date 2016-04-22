%define fedora 0
%define rhel 5
%define dist el5

Summary: 	X Neural Switcher front-end for KDE
Name: 		kxneur
Version: 	0.8
Release: 	0.%{?dist}

Group: 		User Interface/Desktops
License: 	GPL
URL: 		http://xneur.ru
Source: 	%{name}-%{version}.tar.bz2

BuildRoot: 	%{_tmppath}/%{name}-%{version}-root-%(%{__id_u} -n)

BuildRequires: 	qt-devel, kdelibs-devel, xneur-devel = 0.8
Requires: 	xneur = 0.8
Requires: 	kdelibs >= 3.3.0
%if 0%{?fedora} > 4 || 0%{?rhel} > 4
Requires: 	xorg-x11-utils
%else
Requires: 	xorg-x11-tools
%endif
Requires:	perl


%description
X Neural Switcher front-end for KDE
It's program like Punto Switcher, but has other
functionally and features.


%prep
rm -rf $RPM_BUILD_ROOT
%setup -q


%build
make -f admin/Makefile.common dist
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
%makeinstall


%find_lang %{name}


%clean
rm -rf $RPM_BUILD_ROOT


%files -f %{name}.lang
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog NEWS README TODO
%doc %{_docdir}/HTML/*/kxneur/*
%{_bindir}/*
%{_datadir}/apps/kxneur/*
%{_datadir}/pixmaps/*
%{_datadir}/applnk/Utilities/*.desktop
%{_datadir}/autostart/kxneur.desktop


%changelog
* Tue Oct 1 2007 Vadim Likhota <vadim-lvv@yandex.ru> 0.8
- up to 0.8

* Tue Jul 17 2007 Vadim Likhota <vadim-lvv@yandex.ru> 0.6.2-1
- up to 0.6.2
- update & test for centos5 x86_64

* Thu Jun 07 2007 Vadim Likhota <vadim-lvv@yandex.ru> 0.6.1-3
- add apps dir

* Thu May 17 2007 Vadim Likhota <vadim-lvv@yandex.ru> 0.6.1-0
- update to xneur 0.6.1 from svn

* Sat May 12 2007 Vadim Likhota <vadim-lvv@yandex.ru> 0.01
- create test version for fc/asp 
