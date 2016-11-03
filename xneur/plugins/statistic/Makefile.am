## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -DPACKAGE_SYSCONFDIR_DIR=\""@sysconfdir@/xneur"\" -DPACKAGE_SHAREDIR_DIR=\""@datarootdir@/xneur"\"

pkglib_LTLIBRARIES = libxnstatistic.la

libxnstatistic_la_SOURCES = \
	statistic.c

libxnstatistic_la_CFLAGS = -I@top_srcdir@/lib/config -I@top_srcdir@/lib/lib @DEFAULT_CFLAGS@
libxnstatistic_la_LDFLAGS = @X11_LIBS@
	 
libxnstatistic_la_LIBADD = 
