INCLUDES = 	-DPACKAGE_PIXMAPS_DIR=\""@datadir@/gxneur/pixmaps"\" 	\
		-DLOCALEDIR=\""@localedir@"\"				\
		-DPACKAGE_GLADE_FILE_DIR=\""@datadir@/gxneur/glade"\"	\
		-DXNEUR_PLUGIN_DIR=\""@libdir@/xneur"\"

bin_PROGRAMS = gxneur

gxneur_SOURCES =		\
	callbacks.c		\
	callbacks.h		\
	configuration.c		\
	configuration.h		\
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

gxneur_CFLAGS = @X11_CFLAGS@ @GTK_CFLAGS@ @XNEURCONF_CFLAGS@ @GLADE_CFLAGS@ @GCONF_CFLAGS@ @APP_INDICATOR_CFLAGS@ @DEFAULT_CFLAGS@ @XNEURLIB_CFLAGS@

gxneur_LDFLAGS = @X11_LIBS@ @GTK_LIBS@ @XNEURCONF_LIBS@ @GLADE_LIBS@ @GCONF_LIBS@ @APP_INDICATOR_LIBS@ @XNEURLIB_LIBS@ 
