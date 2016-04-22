noinst_LTLIBRARIES = libxnmisc.la

libxnmisc_la_SOURCES =		\
	colors.h		\
	list_char.c		\
	list_char.h		\
	list_void.c		\
	list_void.h		\
	log.c			\
	log.h			\
	text.c			\
	text.h			\
	types.h			\
	utils.c			\
	utils.h			\
	regexp.h		\
	regexp.c		

libxnmisc_la_CFLAGS = @PCRE_CFLAGS@
libxnmisc_la_LDFLAGS = -static @PCRE_LIBS@

configdir = @includedir@/xneur
config_DATA = list_char.h
