Summary: 	X Neural Switcher front-end for KDE
Name: 		kxneur
Version: 	0.01
Release: 	2asp11

Group: 		User Interface/Desktops
License: 	GPL
URL: 		http://www.klv.lg.ua/~vadim/
Source: 	%{name}-%{version}.tar.bz2

BuildRoot: 	%{_tmppath}/%{name}-%{version}-root

BuildRequires: 	qt-devel, kdelibs-devel, xneur-devel = 0.6.0
Requires: 	xneur = 0.6.0
Requires: 	kdelibs >= 3.4.0
Requires: 	xorg-x11-tools


%description
X Neural Switcher front-end for KDE
It's program like Punto Switcher, but has other
functionally and features.


%prep
rm -rf $RPM_BUILD_ROOT
%setup -q


%build
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
%{_datadir}/pixmaps/*
%{_datadir}/applnk/Utilities/*.desktop
%{_datadir}/autostart/kxneur.desktop


%changelog
* Sat May 12 2007 Vadim Likhota <vadim-lvv@yandex.ru> 0.01
- create test version for fc/asp
