## Process this file with automake to produce Makefile.in

INCLUDES = $(GTK_CFLAGS) -DPACKAGE_PIXMAPS_DIR=\""@datadir@/pixmaps"\"

bin_PROGRAMS = gxneur

gxneur_SOURCES =		\
	eggclock.h		\
	eggclock.c		\
	eggtrayicon.c		\
	eggtrayicon.h		\
	interface.c		\
	interface.h		\
	main.c			\
	support.c		\
	support.h		\
	trayicon.c		\
	trayicon.h		\
	config_main.h		\
	list.h			\
	callbacks.h		\
	callbacks.c		\
	misc.h			\
	misc.c

gxneur_LDADD = -lxnconfig $(GTK_LIBS)
