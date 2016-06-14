AC_INIT(kdeneur, 0.19.0)
AC_CONFIG_SRCDIR(configure.ac)
AM_INIT_AUTOMAKE

AC_PREREQ(2.59)
AM_CONFIG_HEADER(config.h)

AC_CONFIG_MACRO_DIR(m4)

AC_PROG_LIBTOOL
AC_PROG_INSTALL

AC_LANG_C
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_MAKE_SET

AC_HEADER_STDC

PKG_PROG_PKG_CONFIG([0.20])

PKG_CHECK_MODULES(X11, [x11 >= 1.1], found_x11="yes", found_x11="no")

if test "x$found_x11" = "xno"; then
	AC_PATH_XTRA
	if test "x$no_x" = "xyes"; then
		AC_MSG_ERROR([Can't locate your X11 installation])
	fi
	X11_CFLAGS="$X_CFLAGS"
	X11_LIBS="$X_LIBS -lX11"
	AC_SUBST(X11_CFLAGS)
	AC_SUBST(X11_LIBS)
else
	X11_LIBS="$X11_LIBS"
	AC_SUBST(X11_LIBS)
fi


PKG_CHECK_MODULES(XNEURCONF, [xnconfig = 0.19.0])
PKG_CHECK_MODULES(XNEURLIB, [xneur = 0.19.0])

PKG_CHECK_MODULES(QTCORE, [QtCore])
PKG_CHECK_MODULES(QTGUI, [QtGui])
PKG_CHECK_MODULES(QTUITOOLS, [QtUiTools])
PKG_CHECK_MODULES(QTDBUS, [QtDBus])

AM_GNU_GETTEXT_VERSION([0.16.1])
AM_GNU_GETTEXT([external])

DEFAULT_CFLAGS="-Wall -Wextra -Werror -g0 -fPIC  -lkdecore -lplasma"
AC_SUBST(DEFAULT_CFLAGS)

AC_OUTPUT([Makefile kdeneur.desktop src/Makefile pixmaps/Makefile i18n/Makefile])

cat << EOF

$PACKAGE $VERSION configured successfully:

	Installing into:        $prefix

	Compiler flags:         $CFLAGS

	X11 compiler flags:	$X11_CFLAGS
	X11 linker flags:	$X11_LIBS

	QtCore compiler flags:	$QTCORE_CFLAGS
	QtCore flags:	$QTCORE_LIBS

	XNeur Config flags:	$XNEURCONF_CFLAGS
	XNeur Config libs:	$XNEURCONF_LIBS
EOF

