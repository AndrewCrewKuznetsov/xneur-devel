AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -DPACKAGE_SYSCONFDIR_DIR=\""@sysconfdir@/xneur"\" -DPACKAGE_SHAREDIR_DIR=\""@datarootdir@/xneur"\"

lib_LTLIBRARIES = libxneur.la

xneur_includedir = $(includedir)/xneur
xneur_include_HEADERS = xneur.h		

libxneur_la_SOURCES =		\
	../config/xnconfig_files.c \
	xneurlib.c	\
	xneur.h		

libxneur_la_CFLAGS = -I@top_srcdir@/lib/misc -I@top_srcdir@/lib/main -I@top_srcdir@/lib/ai -I@top_srcdir@/lib/config -I@top_srcdir@/lib/notify  @DEFAULT_CFLAGS@ @X11_CFLAGS@ @ASPELL_CFLAGS@ @ENCHANT_CFLAGS@
libxneur_la_LDFLAGS = -version-info 19:0:0 @ASPELL_LIBS@ @ENCHANT_LIBS@ @ADDITIONAL_LIBS@
libxneur_la_LIBADD = ../misc/libxnmisc.la ../main/libxnmain.la ../ai/libxnai.la ../notify/libxnnotify.la

EXTRA_DIST = $(config_DATA)

install-data-local:
	$(LDCONFIG)
