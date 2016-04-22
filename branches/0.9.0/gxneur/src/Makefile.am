INCLUDES = 	-DPACKAGE_PIXMAPS_DIR=\""@datadir@/gxneur/pixmaps"\" 	\
		-DLOCALEDIR=\""@localedir@"\"				\
		-DPACKAGE_GLADE_FILE_DIR=\""@datadir@/gxneur/glade"\"

bin_PROGRAMS = gxneur

gxneur_SOURCES =		\
	callbacks.c		\
	callbacks.h		\
	clock.c			\
	clock.h			\
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

gxneur_CFLAGS = @GTK_CFLAGS@ @XNEUR_CFLAGS@ @GLADE_CFLAGS@
gxneur_LDFLAGS = @GTK_LIBS@ @XNEUR_LIBS@ @GLADE_LIBS@
