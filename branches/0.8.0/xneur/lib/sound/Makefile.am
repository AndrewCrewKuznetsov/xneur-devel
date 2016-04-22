noinst_LTLIBRARIES = libxnsound.la

libxnsound_la_SOURCES =		\
	sound.c			\
	sound.h

libxnsound_la_CFLAGS = -I@top_srcdir@/lib/config -I@top_srcdir@/lib/misc @X11_CFLAGS@ @FREEALUT_CFLAGS@ @GSTREAMER_CFLAGS@
libxnsound_la_LDFLAGS = -static @X11_LIBS@ @FREEALUT_LIBS@ @GSTREAMER_LIBS@

