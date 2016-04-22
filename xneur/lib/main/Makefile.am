noinst_LTLIBRARIES = libxnmain.la

xnconfigdir = $(includedir)/xneur

libxnmain_la_SOURCES =		\
	bind_table.c		\
	bind_table.h		\
	buffer.c		\
	buffer.h		\
	event.c			\
	event.h			\
	focus.c			\
	focus.h			\
	keymap.c		\
	keymap.h		\
	plugin.c		\
	plugin.h		\
	program.c		\
	program.h		\
	selection.c		\
	selection.h		\
	switchlang.c		\
	switchlang.h		\
	utils.c			\
	utils.h			\
	window.c		\
	window.h		\
	defines.h


libxnmain_la_CFLAGS = -I@top_srcdir@/lib/config -I@top_srcdir@/lib/misc  -I@top_srcdir@/lib/notify -I@top_srcdir@/lib/ai -I@top_srcdir@/lib/lib -I@top_srcdir@/lib/rec @X11_CFLAGS@ @DEFAULT_CFLAGS@ -DXNEUR_PLUGIN_DIR=\""@libdir@/xneur"\"
libxnmain_la_LDFLAGS = -static @X11_LIBS@  @ADDITIONAL_LIBS@

