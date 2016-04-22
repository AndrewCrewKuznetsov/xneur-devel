## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir)/lib/config -I$(top_srcdir)/lib/misc $(X_CFLAGS)

noinst_LTLIBRARIES = libxnai.la

libxnai_la_SOURCES =		\
	conversion.c		\
	conversion.h		\
	detection.c		\
	detection.h

libxnai_la_LDFLAGS = -static
