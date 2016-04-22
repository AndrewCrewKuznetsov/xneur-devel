dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(xcurf, 0.3)

AM_INIT_AUTOMAKE(xcurf, 0.3)
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC




dnl ***************************************************************************
dnl Internatinalization
dnl ***************************************************************************
GETTEXT_PACKAGE=xcurf
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])



AM_PROG_LIBTOOL


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
fi

found_imlib2_bin_dir=""
for imlib2_dir in /usr/local /usr $prefix
do
	if test -f "$imlib2_dir/bin/imlib2-config"; then
		found_imlib2_bin_dir="$imlib2_dir/bin"
	fi
done

AC_MSG_CHECKING([for imlib2-config bin])
if test "x$found_imlib2_bin_dir" != "x"; then
	AC_MSG_RESULT([found at $found_imlib2_bin_dir])
else
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([Can't locate your imlib2-config bin])
fi

AC_DEFINE(WITH_IMLIB2, 1, [Define if you want imlib2 support])
IMLIB2_LIBS=`imlib2-config --libs`
AC_SUBST(IMLIB2_LIBS)
IMLIB2_CFLAGS=`imlib2-config --cflags`
AC_SUBST(IMLIB2_CFLAGS)


AC_OUTPUT([
Makefile
src/Makefile
pixmaps/Makefile
po/Makefile.in
])
