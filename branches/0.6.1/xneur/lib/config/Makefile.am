INCLUDES = -DPACKAGE_SYSCONFDIR_DIR=\""@sysconfdir@/xneur"\" -DPLUGINS_INSTALL_DIR=\""@libdir@/xneur/plugins"\"

libdir = @libdir@/xneur
lib_LTLIBRARIES = libxnconfig.la

libxnconfig_la_SOURCES =	\
	xnconfig_files.c	\
	xnconfig_files.h	\
	xnconfig.c		\
	xnconfig.h		\
	xnconfig_memory.c	\
	xnconfig_memory.h

libxnconfig_la_CFLAGS = -I@top_srcdir@/lib/misc -I@top_srcdir@/lib/x
libxnconfig_la_LDFLAGS = -version-info 6:0:0
libxnconfig_la_LIBADD = ../misc/libxnmisc.la

configdir = @includedir@/xneur
config_DATA = xnconfig.h

EXTRA_DIST = $(config_DATA)
