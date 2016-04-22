## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir)/lib/config -I$(top_srcdir)/lib/misc -I$(top_srcdir)/lib/ai $(X_CFLAGS)

noinst_LTLIBRARIES = libxnx.la

libxnx_la_SOURCES =		\
	xbtable.c		\
	xbtable.h		\
	xdefines.h		\
	xevent.c		\
	xevent.h		\
	xfocus.c		\
	xfocus.h		\
	xprogram.c		\
	xprogram.h		\
	xselection.c		\
	xselection.h		\
	xstring.c		\
	xstring.h		\
	xswitchlang.c		\
	xswitchlang.h		\
	xutils.c		\
	xutils.h		\
	xwindow.c		\
	xwindow.h

libxnx_la_LDFLAGS = -static
