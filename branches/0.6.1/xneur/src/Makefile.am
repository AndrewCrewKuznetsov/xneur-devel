bin_PROGRAMS = xneur

xneur_SOURCES =			\
	xneur.c			\
	newlang_creation.c	\
	newlang_creation.h

xneur_CFLAGS = -I@top_srcdir@/lib/ai -I@top_srcdir@/lib/config -I@top_srcdir@/lib/x -I@top_srcdir@/lib/misc @X11_CFLAGS@
xneur_LDFLAGS = -L$(top_srcdir)/lib/ai -L$(top_srcdir)/lib/config -L$(top_srcdir)/lib/misc -L$(top_srcdir)/lib/x @X11_LIBS@
xneur_LDADD = ../lib/x/libxnx.la ../lib/ai/libxnai.la ../lib/config/libxnconfig.la ../lib/misc/libxnmisc.la
