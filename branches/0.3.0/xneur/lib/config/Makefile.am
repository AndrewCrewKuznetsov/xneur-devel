## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir)/lib/misc -DPACKAGE_SYSCONFDIR_DIR=\""@sysconfdir@/xneur"\"

lib_LTLIBRARIES = libxnconfig.la

libxnconfig_la_SOURCES =	\
	config_files.c		\
	config_files.h		\
	config_main.c		\
	config_main.h

libxnconfig_la_LIBADD = ../misc/libxnmisc.la
libxnconfig_la_LDFLAGS = -version-info 3:0:0
