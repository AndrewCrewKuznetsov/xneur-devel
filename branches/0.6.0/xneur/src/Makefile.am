INCLUDES = -I$(top_srcdir)/lib/ai -I$(top_srcdir)/lib/config -I$(top_srcdir)/lib/x -I$(top_srcdir)/lib/misc $(X_CFLAGS)

bin_PROGRAMS = xneur

xneur_SOURCES =			\
	xneur.c			\
	newlang_creation.c	\
	newlang_creation.h

xneur_LDFLAGS = -L$(top_srcdir)/lib/ai -L$(top_srcdir)/lib/config -L$(top_srcdir)/lib/misc -L$(top_srcdir)/lib/x $(X_LIBS) `pcre-config --libs`
xneur_LDADD = ../lib/x/libxnx.la ../lib/ai/libxnai.la ../lib/config/libxnconfig.la ../lib/misc/libxnmisc.la -lX11
