INCLUDES = -DPACKAGE_SYSCONFDIR_DIR=\""@sysconfdir@/xneur"\" -DPACKAGE_SHAREDIR_DIR=\""@datarootdir@/xneur"\"

lib_LTLIBRARIES = libxnconfig.la

xnincludedir = $(includedir)/xneur
xninclude_HEADERS = xnconfig.h

noinst_HEADERS =		\
	xnconfig_files.h	

libxnconfig_la_SOURCES =	\
	xnconfig.c		

libxnconfig_la_CFLAGS = -I@top_srcdir@/lib/misc -I@top_srcdir@/lib/main -I@top_srcdir@/lib/lib @DEFAULT_CFLAGS@
libxnconfig_la_LIBADD = ../lib/libxneur.la @ADDITIONAL_LIBS@
libxnconfig_la_LDFLAGS = -version-info 19:0:0

EXTRA_DIST = $(config_DATA)

install-data-local:
	$(LDCONFIG)
