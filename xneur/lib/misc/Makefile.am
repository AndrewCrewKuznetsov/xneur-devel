noinst_LTLIBRARIES = libxnmisc.la

xnconfigdir = $(includedir)/xneur
xnconfig_HEADERS = list_char.h

noinst_HEADERS =		\
	archiver.h		\
	colors.h		\
	debug.h			\
	log.h			\
	mail.h			\
	text.h			\
	types.h			\
	regexp.h

libxnmisc_la_SOURCES =		\
	archiver.c		\
	debug.c			\
	list_char.c		\
	log.c			\
	mail.c			\
	text.c			\
	regexp.c

libxnmisc_la_CFLAGS = @PCRE_CFLAGS@ @DEFAULT_CFLAGS@ -I@top_srcdir@/lib/config -I@top_srcdir@/lib/lib
libxnmisc_la_LDFLAGS = -static @PCRE_LIBS@ @Z_LIBS@ @ADDITIONAL_LIBS@
