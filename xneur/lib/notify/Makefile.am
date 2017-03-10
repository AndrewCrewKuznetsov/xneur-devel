noinst_LTLIBRARIES = libxnnotify.la

libxnnotify_la_SOURCES =	\
	notify.c		\
	notify.h		\
	osd.c			\
	osd.h			\
	popup.c			\
	popup.h			\
	sound.c			\
	sound.h			

libxnnotify_la_CFLAGS = -I@top_srcdir@/lib/config -I@top_srcdir@/lib/misc -I@top_srcdir@/lib/lib 		\
										@X11_CFLAGS@ @DEFAULT_CFLAGS@ 	\
										@FREEALUT_CFLAGS@ 		\
										@GSTREAMER_CFLAGS@ 		\
										@XOSD_CFLAGS@ 			\
										@LIBNOTIFY_CFLAGS@		
libxnnotify_la_LDFLAGS = -static @X11_LIBS@ @ADDITIONAL_LIBS@	\
					@FREEALUT_LIBS@ 	\
					@GSTREAMER_LIBS@ 	\
					@XOSD_LIBS@ 		\
					@LIBNOTIFY_LIBS@	

