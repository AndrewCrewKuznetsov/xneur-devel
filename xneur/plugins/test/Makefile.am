## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -DPACKAGE_SYSCONFDIR_DIR=\""@sysconfdir@/xneur"\" -DPACKAGE_SHAREDIR_DIR=\""@datarootdir@/xneur"\"

pkglib_LTLIBRARIES = libxntest.la

libxntest_la_SOURCES = \
	test.c

libxntest_la_CFLAGS = -I@top_srcdir@/lib/config -I@top_srcdir@/lib/lib @DEFAULT_CFLAGS@
libxntest_la_LDFLAGS = @X11_LIBS@
	 
libxntest_la_LIBADD = 
