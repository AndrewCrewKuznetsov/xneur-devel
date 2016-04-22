noinst_LTLIBRARIES = libxnai.la

libxnai_la_SOURCES =		\
	conversion.c		\
	conversion.h		\
	detection.c		\
	detection.h

libxnai_la_CFLAGS = -I@top_srcdir@/lib/config -I@top_srcdir@/lib/misc -I@top_srcdir@/lib/x @X11_CFLAGS@ @ASPELL_CFLAGS@
libxnai_la_LDFLAGS = -static @X11_LIBS@ @ASPELL_LIBS@
libxnai_la_LIBADD = ../misc/libxnmisc.la
