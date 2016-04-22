noinst_LTLIBRARIES = libxnai.la

libxnai_la_SOURCES =		\
	conversion.c		\
	conversion.h		\
	detection.c		\
	detection.h		

libxnai_la_CFLAGS = -I@top_srcdir@/lib/config -I@top_srcdir@/lib/misc -I@top_srcdir@/lib/main -I@top_srcdir@/lib/lib @X11_CFLAGS@ @ASPELL_CFLAGS@ @DEFAULT_CFLAGS@
libxnai_la_LDFLAGS = -static @X11_LIBS@ @ASPELL_LIBS@ @ADDITIONAL_LIBS@

