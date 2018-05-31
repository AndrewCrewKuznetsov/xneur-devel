AC_INIT(gxneur, 0.20.0)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PATH_XTRA

GLIB_GSETTINGS

AC_CONFIG_MACRO_DIRS([m4])

AR_FLAGS=cr 

dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
m4_ifdef([IT_PROG_INTLTOOL],[
IT_PROG_INTLTOOL([0.35.0])],[
AC_MSG_ERROR("You need to install intltool in order to build gxneur. Install intltool and run ./autogen.sh again.")
])

GETTEXT_PACKAGE=gxneur
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT

LT_INIT

PKG_CHECK_MODULES(XNEURCONF, [xnconfig = 0.20.0])
PKG_CHECK_MODULES(XNEURLIB, [xneur = 0.20.0])

PKG_CHECK_MODULES(X11, [x11 >= 1.1], found_x11="yes", found_x11="no")
if test "x$found_x11" = "xno"; then
	AC_PATH_XTRA

	if test "x$no_x" = "xyes"; then
		AC_MSG_ERROR([Cant locate your X11 installation])
	fi

	X11_CFLAGS="$X_CFLAGS"
	X11_LIBS="$X_LIBS -lX11"

	AC_SUBST(X11_CFLAGS)
	AC_SUBST(X11_LIBS)
else
	X11_LIBS="$X11_LIBS"
	AC_SUBST(X11_LIBS)
fi

#PKG_CHECK_MODULES(GTK, [gtk+-3.0], found_gtk="yes", found_gtk="no")
#if test "x$found_gtk" = "xno"; then
	PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= 2.20.0], found_gtk="yes", found_gtk="no")
	if test "x$found_gtk" = "xno"; then
		PKG_CHECK_MODULES(GTK, [gtk+-2.0 < 2.20.0])
	else
		AC_DEFINE(WITH_GTK_NEW, 1, [Define if you use GTK+ >= 2.20.0 support])
	fi
#else
#	AC_DEFINE(WITH_GTK_NEW, 1, [Define if you use GTK+ >= 2.20.0 support])
#fi


AC_ARG_WITH(appindicator, [  --without-appindicator	Compile without AppIndicator library support])
if test "x$with_appindicator" != "xno"; then
	APPINDICATOR_REQUIRED=0.2.9

	PKG_CHECK_MODULES(APP_INDICATOR, [appindicator-0.1 >= $APPINDICATOR_REQUIRED], found_appindicator="yes", found_appindicator="no")
	if test "x$found_appindicator" = "xno"; then
		PKG_CHECK_MODULES(APP_INDICATOR, [appindicator-0.1 < $APPINDICATOR_REQUIRED], found_deprec_appindicator="yes", found_deprec_appindicator="no")

		if test "x$found_deprec_appindicator" = "xyes"; then
			AC_DEFINE(HAVE_APP_INDICATOR, 1, [Have AppIndicator])
			AC_DEFINE(HAVE_DEPREC_APP_INDICATOR, 1, [Have Deprecated AppIndicator])
		fi 
	else
		AC_DEFINE(HAVE_APP_INDICATOR, 1, [Have AppIndicator])
	fi
fi

AC_DEFINE(PACKAGE_GCONF_DIR, "/apps/" PACKAGE, [Define GConf directory for settings])
AC_DEFINE(PACKAGE_GSETTINGS_DIR, "com." PACKAGE ".settings", [Define GSettings paths for settings])

AC_DEFINE(KB_PROP_COMMAND, "/usr/bin/gnome-control-center region layouts", [Define keyboard properties command])

#PKG_CHECK_MODULES(GCONF, [gconf-2.0 >= 2.0])
AC_ARG_WITH(gconf, [  --without-gconf	Compile without GConf support])
if test "x$with_gconf" != "xno"; then
	PKG_CHECK_MODULES(GCONF, [gconf-2.0 >= 2.0], found_gconf="yes", found_gconf="no")
	if test "x$found_gconf" = "xyes"; then
		AC_DEFINE(HAVE_GCONF, 1, [Have GConf])
	fi
fi

DEFAULT_CFLAGS="-Wall -Wextra -Werror -g0 -fPIC -std=gnu99"
AC_SUBST(DEFAULT_CFLAGS)

AC_OUTPUT([Makefile gxneur.desktop data/Makefile src/Makefile pixmaps/Makefile ui/Makefile po/Makefile.in])

cat << EOF

$PACKAGE $VERSION configured successfully:

	Installing into:        $prefix

	Compiler flags:         $CFLAGS

	X11 compiler flags:	$X11_CFLAGS
	X11 linker flags:	$X11_LIBS

	GTK compiler flags:	$GTK_CFLAGS
	GTK linker flags:	$GTK_LIBS

	XNeur Config flags:	$XNEURCONF_CFLAGS
	XNeur Config libs:	$XNEURCONF_LIBS

	XNeur flags:		$XNEURLIB_CFLAGS
	XNeur libs:		$XNEURLIB_LIBS

EOF

#if (test "x$found_gtk2" != "xno") || (test "x$found_gtk3" != "xno"); then
if (test "x$found_gtk2" != "xno"); then
cat <<EOF
	Use GTK+ >= 2.20.0 support

EOF
fi

if test "x$with_appindicator" != "xno"; then
	if test "x$found_appindicator" != "xno"; then
cat <<EOF
	Use App Indicator 0.1 >= $APPINDICATOR_REQUIRED support

EOF
	else
 	if test "x$found_deprec_appindicator" != "xno"; then
cat <<EOF
	Use App Indicator 0.1 < $APPINDICATOR_REQUIRED support

EOF
 	fi
	fi
fi
