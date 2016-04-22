INCLUDES = 	-DLOCALEDIR=\""@localedir@"\"

bin_PROGRAMS = xneur

xneur_SOURCES =			\
	xneur.c			\
	newlang_creation.c	\
	newlang_creation.h

xneur_CFLAGS = -I@top_srcdir@/lib/ai  -I@top_srcdir@/lib/lib  -I@top_srcdir@/lib/config -I@top_srcdir@/lib/main -I@top_srcdir@/lib/misc -I@top_srcdir@/lib/notify  @X11_CFLAGS@ @DEFAULT_CFLAGS@

xneur_LDFLAGS = -L$(top_srcdir)/lib/lib -L$(top_srcdir)/lib/ai -L$(top_srcdir)/lib/config -L$(top_srcdir)/lib/misc -L$(top_srcdir)/lib/main -L$(top_srcdir)/lib/notify  @X11_LIBS@ @ADDITIONAL_LIBS@

xneur_LDADD = ../lib/lib/libxneur.la ../lib/config/libxnconfig.la ../lib/notify/libxnnotify.la

