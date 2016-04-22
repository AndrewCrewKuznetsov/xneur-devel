%define fedora 0
%define rhel 5
%define dist el5

Summary: 	GTK frontend for X Neural Switcher
Name: 		gxneur
Version: 	0.8
Release: 	0.%{?dist}

Group: 		Applications/Office
License: 	GPL
URL: 		http://www.xneur.ru
Source: 	%{name}-%{version}.tar.bz2
Source1: 	gxneur.desktop

BuildRoot: 	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: 	gtk2-devel, xneur-devel = %{version}
Requires: 	xneur = %{version}, gtk2
%if 0%{?fedora} > 4 || 0%{?rhel} > 4
Requires: 	xorg-x11-utils
%else
Requires: 	xorg-x11-tools
%endif


%description
GTK frontend for X Neural Switcher.
It's program like Punto Switcher, but has other
functionally and features.


%prep
rm -rf $RPM_BUILD_ROOT
%setup -q


%build
./autogen.sh
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
%makeinstall

desktop-file-install --vendor fedora			  	\
	--dir ${RPM_BUILD_ROOT}%{_datadir}/applications		\
	%{SOURCE1}
# fix bug in config files (in xneur this bug fixed)
mkdir -p ${RPM_BUILD_ROOT}/usr/local/share/
ln -s /usr/share/locale ${RPM_BUILD_ROOT}/usr/local/share/locale
ln -s /usr/share/pixmaps ${RPM_BUILD_ROOT}/usr/local/share/pixmaps

%find_lang %{name}


%clean
rm -rf $RPM_BUILD_ROOT


%files -f %{name}.lang
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog NEWS README TODO
%{_bindir}/*
%{_datadir}/pixmaps/*
%{_datadir}/applications/*.desktop
/usr/local/share/*

%changelog
* Thu Oct 11 2007 vadim Likhota <vadim-lvv@yandex.ru> 0.8
- update to 0.8

* Tue Jul 17 2007 Vadim Likhota <vadim-lvv@yandex.ru> 0.6.2-1
- update+test for centos5

* Wed Jul 04 2007 Vadim Likhota <vadim-lvv@yandex.ru> 0.6.2-0
- 0.6.2

* Mon May 28 2007 Vadim Likhota <vadim-lvv@yandex.ru> 0.6.1-1
- 0.6.1

* Mon Apr 23 2007 Arkady L. Shane <ashejn@yandex-team.ru> 0.6-1
- 0.6.0

* Sat Mar 10 2007 Arkady L. Shane <ashejn@yandex-team.ru> 0.5-1
- 0.5.0

* Tue Jan 23 2007 Arkady L. Shane <ashejn@yandex-team.ru> 0.4.0-1
- rebuild for FC6
- cleanup spec

* Fri Jan 5 2007 Nik <niktr@mail.ru>
- adopted spec from ALT for FC6
- updated to svn version dated 03012007
- minor changes in spec and desktop files

* Thu Dec 21 2006 Vitaly Lipatov <lav@altlinux.ru> 0.3.0-alt1
- new version (0.3.0)

* Sun Oct 15 2006 Vitaly Lipatov <lav@altlinux.ru> 0.1.0_1-alt1
- initial build for Sisyphus
