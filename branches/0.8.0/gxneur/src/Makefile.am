INCLUDES = -DPACKAGE_PIXMAPS_DIR=\""@datadir@/pixmaps"\" -DLOCALEDIR=\""@localedir@"\"

bin_PROGRAMS = gxneur

gxneur_SOURCES =		\
	callbacks.c		\
	callbacks.h		\
	clock.c			\
	clock.h			\
	interface.c		\
	interface.h		\
	main.c			\
	misc.c			\
	misc.h			\
	support.c		\
	support.h		\
	tray_widget.c		\
	tray_widget.h		\
	trayicon.c		\
	trayicon.h		\
	xkb.c			\
	xkb.h

gxneur_CFLAGS = @GTK_CFLAGS@ @XNEUR_CFLAGS@
gxneur_LDFLAGS = @GTK_LIBS@ @XNEUR_LIBS@
