INCLUDES = -I$(top_srcdir)/lib/misc -DPACKAGE_SYSCONFDIR_DIR=\""@sysconfdir@/xneur"\"

lib_LTLIBRARIES = libxnconfig.la

libxnconfig_la_SOURCES =	\
	xnconfig_files.c	\
	xnconfig_files.h	\
	xnconfig.c		\
	xnconfig.h		\
	xnconfig_memory.c	\
	xnconfig_memory.h

libxnconfig_la_LIBADD = ../misc/libxnmisc.la
libxnconfig_la_LDFLAGS = -version-info 3:0:0

configdir = $(includedir)/xneur
config_DATA = xnconfig.h

EXTRA_DIST = $(config_DATA)
