ACLOCAL_AMFLAGS = -I m4

SUBDIRS =			\
	src			\
	pixmaps			\
	po			\
	ui

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

Multimediadir = $(datadir)/applications
Multimedia_in_files = gxneur.desktop.in
Multimedia_DATA = $(Multimedia_in_files:.desktop.in=.desktop)

EXTRA_DIST = $(noinst_DATA) m4/ChangeLog gxneur.1 $(Multimedia_DATA) gxneur.desktop.in

man_MANS = gxneur.1

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
