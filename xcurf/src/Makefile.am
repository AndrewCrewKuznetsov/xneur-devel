INCLUDES = -DPACKAGE_SYSCONFDIR_DIR=\""@sysconfdir@/xcurf"\" -DPACKAGE_SHAREDIR_DIR=\""@datarootdir@/xcurf"\"

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" 

AM_CFLAGS =\
	 -Wall\
	 -g\
	 @X11_CFLAGS@\
	 @IMLIB2_CFLAGS@

bin_PROGRAMS = xcurf

xcurf_SOURCES = \
	main.c

xcurf_LDFLAGS = @X11_LIBS@ @IMLIB2_LIBS@

xcurf_LDADD = 

