AC_INIT(xneur, 0.19.0)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PATH_XTRA

AC_CONFIG_MACRO_DIRS([m4])

AR_FLAGS=cr 

dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
IT_PROG_INTLTOOL([0.35.0])
GETTEXT_PACKAGE=xneur
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT

LT_INIT

AC_CHECK_HEADERS([sys/io.h])

AC_CHECK_HEADERS([zlib.h], found_z="yes", found_z="no")
if test "x$found_z" != "xno"; then
	Z_LIBS="-lz"
	AC_SUBST(Z_LIBS)
fi

PKG_CHECK_MODULES(X11, [x11 >= 1.1], found_x11="yes", found_x11="no")
PKG_CHECK_MODULES(Xi, [xi])
if test "x$found_x11" = "xno"; then
	AC_PATH_XTRA

	if test "x$no_x" = "xyes"; then
		AC_MSG_ERROR([Can't locate your X11 installation])
	fi

	X11_CFLAGS="$X_CFLAGS"
#	AC_CHECK_LIB(Xtst, XTestFakeMotionEvent, LIBXTST="-lXtst",[AC_CHECK_LIB(Xext,XTestFakeMotionEvent , LIBXTST="-lXext")])
#	if test "x$LIBXTST" = "x" ; 
#	then
#		echo " **** ERROR **** "
#		echo ""
#		echo "Could not locate the needed X11 extensions:"
#		echo ""
#		echo "   RECORD"
#		echo "   XTest"
#		echo ""
#		echo "Make sure you have them installed before building Xneur"
#		echo ""
#		echo "  ... on Debian based distros do: apt-get install libxtst-dev"
#		echo " "
#		exit 1
#	fi
#	X11_LIBS="$X_LIBS -lX11 $LIBXTST $LIBXI"
	X11_LIBS="$X_LIBS -lX11 -lXi"

	AC_SUBST(X11_CFLAGS)
	AC_SUBST(X11_LIBS)
else
#	AC_CHECK_LIB(Xtst, XTestFakeMotionEvent, LIBXTST="-lXtst",[AC_CHECK_LIB(Xext,XTestFakeMotionEvent , LIBXTST="-lXext")])
#	if test "x$LIBXTST" = "x" ; 
#	then
#		echo " **** ERROR **** "
#		echo ""
#		echo "Could not locate the needed X11 extensions:"
#		echo ""
#		echo "   RECORD"
#		echo "   XTest"
#		echo ""
#		echo "Make sure you have them installed before building Xneur"
#		echo ""
#		echo "  ... on Debian based distros do: apt-get install libxtst-dev"
#		echo " "
#		exit 1
#	fi
#	X11_LIBS="$X_LIBS -lX11 $LIBXTST $LIBXI"
	X11_LIBS="$X_LIBS -lX11 -lXi"
	AC_SUBST(X11_LIBS)
fi

#ADDITIONAL_LIBS="-lpthread -ldl -lXtst -lXext"
ADDITIONAL_LIBS="-lpthread -ldl"
AC_SUBST(ADDITIONAL_LIBS)

AC_ARG_WITH(sound, [  --with-sound[[=ARG]]	  Compile with sound support ARG=[[yes|no|gstreamer|openal|aplay]] [[default=yes]]])
if test "x$with_sound" != "xno"; then
	if test "x$with_sound" = "x"; then
		with_sound="gstreamer"
	fi

	case $with_sound in
		gstreamer|yes)
			PKG_CHECK_MODULES(GSTREAMER, [gstreamer-1.0 >= 1.2.4], found_gst="yes", found_gst="no")
			if test "x$found_gst" = "xno"; then
				PKG_CHECK_MODULES(GSTREAMER, [gstreamer-0.10 >= 0.10.6])
			fi
			AC_DEFINE(WITH_GSTREAMER, 1, [Define if you want gstreamer sound support])
			;;
		openal)
			PKG_CHECK_MODULES(FREEALUT, [freealut >= 1.0.1])
			AC_DEFINE(WITH_OPENAL, 1, [Define if you want openal sound support])
			;;
		aplay)
			found_aplay_bin_dir=""
			for aplay_dir in /usr/local /usr $prefix
			do
				if test -f "$aplay_dir/bin/aplay"; then
					found_aplay_bin_dir="$aplay_dir/bin"
				fi
			done

			AC_MSG_CHECKING([for aplay bin])
			if test "x$found_aplay_bin_dir" != "x"; then
				AC_MSG_RESULT([found at $found_aplay_bin_dir])
			else
				AC_MSG_RESULT([no])
				AC_MSG_ERROR([Can't locate your aplay bin])
			fi

			AC_DEFINE(WITH_APLAY, 1, [Define if you want aplay support])
			APLAY_LIBS="-lpthread"
			AC_SUBST(APLAY_LIBS)
			;;
		*)
			AC_MSG_ERROR([Unsupported sound system selected, --with-sound=[[yes|no|gstreamer|openal|aplay]]])
			;;
	esac

	AC_DEFINE(WITH_SOUND, 1, [Define if you want sound support])
fi

AC_ARG_WITH(debug, [  --with-debug		  Compile with debug support])
if test "x$with_debug" == "xyes"; then
	AC_DEFINE(WITH_DEBUG, 1, [Define if you want debug support])
fi

PKG_CHECK_MODULES(PCRE, [libpcre >= 5.0])

AC_ARG_WITH(keylogger, [  --without-keylogger	  Compile without keylogger function])
if test "x$with_keylogger" != "xno"; then
	AC_DEFINE(WITH_KEYLOGGER, 1, [Define if you want keylogger function])
fi

AC_ARG_WITH(spell, [  --with-spell[[=ARG]]	  Compile with sound support ARG=[[yes|no|enchant|aspell]] [[default=yes]]])
if test "x$with_spell" != "xno"; then
	if test "x$with_spell" = "x"; then
		with_spell="enchant"
	fi

	case $with_spell in
		enchant|yes)
			PKG_CHECK_MODULES(ENCHANT, [enchant >= 1.0])
			AC_DEFINE(WITH_ENCHANT, 1, [Define if you want enchant support])
			;;
		aspell)
			AC_ARG_WITH(aspell, [  --without-aspell	  Compile without aspell support])
			if test "x$with_aspell" != "xno"; then
				found_aspell_include_dir=""
				found_aspell_lib_dir=""
				for aspell_dir in /usr/local /usr $prefix
				do
					if test -f "$aspell_dir/include/aspell.h"; then
						found_aspell_include_dir="$aspell_dir/include"
					fi

					if test -f "$aspell_dir/lib/libaspell.so"; then
						found_aspell_lib_dir="$aspell_dir/lib"
					fi
					if test -f "$aspell_dir/lib64/libaspell.so"; then
						found_aspell_lib_dir="$aspell_dir/lib64"
					fi
				done

				AC_MSG_CHECKING([for aspell header])
				if test "x$found_aspell_include_dir" != "x"; then
					AC_MSG_RESULT([found at $found_aspell_include_dir])
					ASPELL_CFLAGS="-I$found_aspell_include_dir"
				else
					AC_MSG_RESULT([no])
					AC_MSG_ERROR([Can't locate your aspell headers])
				fi

				AC_MSG_CHECKING([for aspell library])
				if test "x$found_aspell_lib_dir" != "x"; then
					AC_MSG_RESULT([found at $found_aspell_lib_dir])
					ASPELL_LIBS="-L$found_aspell_lib_dir -laspell"
				else
					AC_MSG_RESULT([no])
					AC_MSG_ERROR([Can't locate your aspell library])
				fi

				AC_DEFINE(WITH_ASPELL, 1, [Define if you want aspell support])
				AC_SUBST(ASPELL_CFLAGS)
				AC_SUBST(ASPELL_LIBS)
			fi
			;;
		*)
			AC_MSG_ERROR([Unsupported spell system selected, --with-spell=[[yes|no|enchant|aspell]]])
			;;
	esac
fi

AC_ARG_WITH(xosd, [  --without-xosd	  Compile without xosd support])
if test "x$with_xosd" != "xno"; then
	found_xosd_bin_dir=""
	for xosd_dir in /usr/local /usr $prefix
	do
		if test -f "$xosd_dir/bin/xosd-config"; then
			found_xosd_bin_dir="$xosd_dir/bin"
		fi
	done

	AC_MSG_CHECKING([for xosd-config bin])
	if test "x$found_xosd_bin_dir" != "x"; then
		AC_MSG_RESULT([found at $found_xosd_bin_dir])
	else
		AC_MSG_RESULT([no])
		AC_MSG_ERROR([Can't locate your xosd-config bin])
	fi

	AC_DEFINE(WITH_XOSD, 1, [Define if you want xosd support])
	XOSD_LIBS=`xosd-config --libs`
	XOSD_CFLAGS=`xosd-config --cflags`

	AC_SUBST(XOSD_CFLAGS)
	AC_SUBST(XOSD_LIBS)
fi

AC_ARG_WITH(libnotify, [  --without-libnotify	  Compile without libnotify message support])
if test "x$with_libnotify" != "xno"; then

	AC_ARG_WITH(gtk, [  --with-gtk[[=ARG]]	  Compile with additional GTK support ARG=[[yes|no|gtk2|gtk3]] [[default=no]]])
	if test "x$with_gtk" != "xno"; then
		if test "x$with_gtk" = "x"; then
			with_gtk="gtk3"
		fi
	
		case $with_gtk in
			gtk2)
				PKG_CHECK_MODULES(GTK, [gtk+-2.0], found_gtk="yes", found_gtk="no")
				;;
			gtk3|yes)
				PKG_CHECK_MODULES(GTK, [gtk+-3.0], found_gtk="yes", found_gtk="no")
				;;
			*)
				AC_MSG_ERROR([Unsupported Gtk+ selected, --with-gtk=[[yes|no|gtk2|gtk3]]])
				;;
		esac
	
		PKG_CHECK_MODULES(LIBNOTIFY, [libnotify >= 0.4.0])
		AC_DEFINE(WITH_LIBNOTIFY, 1, [Define if you want libnotify message support])
	fi
fi

AC_ARG_WITH(plugins, [  --without-plugins	  Compile without plugins support])
if test "x$with_plugins" != "xno"; then
	AC_DEFINE(WITH_PLUGINS, 1, [Define if you want plugins support])
fi

DEFAULT_CFLAGS="-Wall -Wextra -Werror -g0 -std=gnu99 -fPIC"
AC_SUBST(DEFAULT_CFLAGS)

AC_OUTPUT([
pixmaps/Makefile 
plugins/Makefile 
plugins/test/Makefile 
plugins/statistic/Makefile 
share/Makefile 
share/sounds/Makefile 
share/languages/Makefile 
share/languages/by/Makefile 
share/languages/bg/Makefile 
share/languages/cz/Makefile 
share/languages/de/Makefile 
share/languages/ge/Makefile 
share/languages/gr/Makefile 
share/languages/en/Makefile 
share/languages/es/Makefile 
share/languages/ee/Makefile 
share/languages/fr/Makefile 
share/languages/gb/Makefile 
share/languages/am/Makefile 
share/languages/kz/Makefile 
share/languages/lt/Makefile 
share/languages/lv/Makefile 
share/languages/pl/Makefile 
share/languages/ro/Makefile 
share/languages/ru/Makefile 
share/languages/ua/Makefile 
share/languages/us/Makefile 
share/languages/uz/Makefile 
etc/Makefile 
lib/Makefile 
lib/lib/Makefile 
lib/ai/Makefile 
lib/config/Makefile 
lib/misc/Makefile 
lib/notify/Makefile 
lib/main/Makefile 
src/Makefile 
po/Makefile.in 
Makefile 
xnconfig.pc 
xneur.pc])

cat << EOF

$PACKAGE $VERSION configured successfully:

	Installing into:		$prefix

	Compiler flags:			$DEFAULT_CFLAGS
	Linker flags:			$DEFAULT_LDFLAGS

	X11 compiler flags:		$X11_CFLAGS
	X11 linker flags:		$X11_LIBS

	Pcre compiler flags:		$PCRE_CFLAGS
	Pcre linker flags:		$PCRE_LIBS

	Additional libs:		$ADDITIONAL_LIBS

EOF

case $with_sound in
	gstreamer|yes)
cat << EOF
	Gstreamer compiler flags:	$GSTREAMER_CFLAGS
	Gstreamer linker flags:		$GSTREAMER_LIBS

EOF
		;;
	openal)
cat << EOF
	FreeALUT compiler flags:	$FREEALUT_CFLAGS
	FreeALUT linker flags:		$FREEALUT_LIBS

EOF
		;;
	aplay)
cat << EOF
	Aplay support enable.

EOF
		;;
esac

case $with_spell in
	enchant|yes)
cat <<EOF
	Enchant compiler flags:		$ENCHANT_CFLAGS
	Enchant linker flags:		$ENCHANT_LIBS

EOF
	;;
	aspell)
cat <<EOF
	Aspell compiler flags:		$ASPELL_CFLAGS
	Aspell linker flags:		$ASPELL_LIBS

EOF
	;;
esac


if test "x$with_xosd" != "xno"; then
cat <<EOF
	Xosd compiler flags:		$XOSD_CFLAGS
	Xosd linker flags:		$XOSD_LIBS

EOF
fi

if test "x$with_popup" != "xno"; then
cat <<EOF
	Libnotify compiler flags:	$LIBNOTIFY_CFLAGS
	Libnotify linker flags:		$LIBNOTIFY_LIBS
EOF
fi

if test "x$with_popup" != "xno"; then
	if test "x$found_gtk" == "xyes"; then
cat <<EOF
	GTK compiler flags:		$GTK_CFLAGS
	GTK linker flags:		$GTK_LIBS
EOF
	fi
fi

