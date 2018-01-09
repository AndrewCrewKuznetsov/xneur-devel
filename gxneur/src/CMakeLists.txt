set (SOURCES
    callbacks.c
    callbacks.h
    configuration.c
    configuration.h
    main.c
    misc.c
    misc.h
    support.h
    tray_widget.c
    tray_widget.h
    trayicon.c
    trayicon.h
    xkb.c
    xkb.h
)

add_executable(gxneur ${SOURCES})
target_compile_definitions(gxneur PRIVATE VERSION=\"${PROJECT_VERSION}\") # VERSION definition is used in code
target_compile_definitions(gxneur PRIVATE PACKAGE=\"${PROJECT_NAME}\") # PACKAGE definition is used in code

set (PKGDATADIR /usr/share/${PROJECT_NAME})
target_compile_definitions (gxneur PRIVATE PACKAGE_PIXMAPS_DIR=\"${PKGDATADIR}/pixmaps\")
target_compile_definitions (gxneur PRIVATE LOCALEDIR=\"${PKGDATADIR}/locale\")
target_compile_definitions (gxneur PRIVATE PACKAGE_UI_FILE_DIR=\"${PKGDATADIR}/ui\")
target_compile_definitions (gxneur PRIVATE XNEUR_PLUGIN_DIR=\"${CMAKE_INSTALL_PREFIX}/lib/xneur\")
target_compile_definitions (gxneur PRIVATE KB_PROP_COMMAND="/usr/bin/gnome-control-center region layouts")
target_compile_definitions (gxneur PRIVATE GETTEXT_PACKAGE="gxneur") # why interface is still half-english?

# target_compile_definitions (gxneur PRIVATE HAVE_CONFIG_H=1)
# target_include_directories(gxneur PRIVATE "..") # for config.h

if(APPINDICATOR)
	set(APPINDICATOR_REQUIRED 0.2.9)
	find_package(PkgConfig)
	pkg_check_modules(APP_INDICATOR REQUIRED appindicator-0.1)
	if(APP_INDICATOR_VERSION>=${APPINDICATOR_REQUIRED})
		target_compile_definitions (gxneur PRIVATE HAVE_APP_INDICATOR=1)
	else()
		target_compile_definitions (gxneur PRIVATE HAVE_APP_INDICATOR=1)
		target_compile_definitions (gxneur PRIVATE HAVE_DEPREC_APP_INDICATOR=1)
	endif()
	target_include_directories(gxneur PRIVATE ${APP_INDICATOR_INCLUDE_DIRS})
endif(APPINDICATOR)

if(GCONF)
	find_package(PkgConfig)
	pkg_check_modules(GCONF REQUIRED gconf-2.0>=2.0)
    target_compile_definitions (gxneur PRIVATE HAVE_GCONF=1)
    set (mySTRING " \"/apps/\" PACKAGE")
#     message (mySTRING ${mySTRING})
    target_compile_definitions (gxneur PRIVATE PACKAGE_GCONF_DIR=${mySTRING}) # why just " \"/apps/\" PACKAGE" do not works?
	target_include_directories(gxneur PRIVATE ${GCONF_INCLUDE_DIRS})
endif(GCONF)

find_package(PkgConfig)
pkg_check_modules(GTK REQUIRED gtk+-2.0)
if(GTK_VERSION>=2.20.0)
	target_compile_definitions (gxneur PRIVATE WITH_GTK_NEW=1)
endif()
include_directories(${GTK_INCLUDE_DIRS})

target_link_libraries(gxneur X11 dl ${GTK_LIBRARIES} xnconfig ${GCONF_LIBRARIES} ${APP_INDICATOR_LIBRARIES} xneur)
install(TARGETS gxneur DESTINATION bin)
