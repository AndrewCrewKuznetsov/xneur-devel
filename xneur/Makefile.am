ACLOCAL_AMFLAGS = -I m4

SUBDIRS =			\
	lib			\
	src			\
	share			\
	etc			\
	po			\
	pixmaps			\
	plugins			

AUTOMAKE_OPTIONS =		\
	1.9			\
	dist-bzip2

noinst_DATA =			\
	README			\
	COPYING			\
	AUTHORS			\
	INSTALL			\
	NEWS			\
	TODO			\
	ChangeLog

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = xnconfig.pc xneur.pc

EXTRA_DIST = $(noinst_DATA) $(pkgconfig_DATA) xnconfig.pc.in xneur.pc.in xneur.1 xneurrc.5
man_MANS = xneur.1 xneurrc.5

check-gettext:
	@if test x$(USE_NLS) != "xyes" ; then echo "Missing gettext. Rerun configure and check for" \
	"'checking whether to use NLS... yes'!" ; exit 1 ; fi

update-po: check-gettext
	@find $(srcdir)/src/ -name "*.cpp" -print | sort > $(srcdir)/po/POTFILES.in.2 ; \
	if diff $(srcdir)/po/POTFILES.in $(srcdir)/po/POTFILES.in.2 >/dev/null 2>&1 ; then \
		rm -f $(srcdir)/po/POTFILES.in.2 ; \
	else \
		mv $(srcdir)/po/POTFILES.in.2 $(srcdir)/po/POTFILES.in ; \
	fi
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-po

update-gmo: check-gettext
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

force-update-gmo: check-gettext
	touch po/*.po
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

.PHONY: check-gettext update-po update-gmo force-update-gmo
