SUBDIRS =			\
	etc			\
	lib			\
	src			\
	share

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

EXTRA_DIST = $(noinst_DATA) $(pkgconfig_DATA) xnconfig.pc.in xneur.1 xneurrc.1
man_MANS = xneur.1 xneurrc.1

