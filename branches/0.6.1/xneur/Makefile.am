SUBDIRS =			\
	etc			\
	lib			\
	src

AUTOMAKE_OPTIONS =		\
	1.9			\
	dist-bzip2

noinst_DATA =			\
	README			\
	COPYING			\
	AUTHORS			\
	ChangeLog		\
	INSTALL			\
	NEWS			\
	TODO

pkgconfigdir = @libdir@/pkgconfig
pkgconfig_DATA = xnconfig.pc

EXTRA_DIST = $(noinst_DATA) $(pkgconfig_DATA) xnconfig.pc.in
