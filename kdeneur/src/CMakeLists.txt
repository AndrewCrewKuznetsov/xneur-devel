set (SOURCES
    main.cpp
    kxneurtray.cpp
    frmabout.cpp
    frmsettings.cpp
    kdeneur.cpp
    xkb.c
    frmaddabbreviature.cpp
    xneurconfig.cpp
    getnameapp.cpp
    ruleschange.cpp
    addrules.cpp
    edithotkey.cpp
    adduseraction.cpp
)

set (MOC_HEADERS
    kxneurtray.h
    frmabout.h
    frmsettings.h
    tabbar.h
    kdeneur.h
    xkb.h
    frmaddabbreviature.h
    xneurconfig.h
    getnameapp.h
    ruleschange.h
    addrules.h
    edithotkey.h
    adduseraction.h
)

set (UIS
    frmabout.ui
    frmsettings.ui
    frmaddabbreviature.ui
    getnameapp.ui
    ruleschange.ui
    addrules.ui
    edithotkey.ui
    adduseraction.ui
)

set (RESOURCES
    resursrc.qrc
)

# Next lines needed for building all Qt projects
find_package (Qt4 REQUIRED)
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )
include_directories( ${CMAKE_BINARY_DIR} )

# Next, using precompiler, compiler and linker

# using Qt meta-system (precompiler)
QT4_ADD_RESOURCES( RES_SOURCES ${RESOURCES} )
QT4_WRAP_UI( UI_HEADERS ${UIS} )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )

# compile
if ("${TR_FILES}" STREQUAL "")
	message (WARNING "TR_FILES variable is empty. Did you set it correctly?")
endif()

QT4_ADD_TRANSLATION (QM_FILES ${TR_FILES})
#  string(ASCII 27 Esc)
#  message ("${Esc}[31m наш tr_files в src равен `${TR_FILES}` ${Esc}[m")

add_executable(kdeneur ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS} ${QM_FILES})
target_link_libraries(kdeneur X11 kdeui kdecore xneur xnconfig plasma ${QT_LIBRARIES})

set (PKGDATADIR /usr/share/${PROJECT_NAME})
target_compile_definitions (kdeneur PRIVATE PACKAGE_PIXMAPS_DIR=\"${PKGDATADIR}/pixmaps\")
target_compile_definitions (kdeneur PRIVATE LOCALEDIR=\"${PKGDATADIR}/i18n\")
target_compile_definitions (kdeneur PRIVATE XNEUR_PLUGIN_DIR=\"${CMAKE_INSTALL_PREFIX}/lib/xneur\")
target_compile_definitions (kdeneur PRIVATE SHAREDIR=\"${PKGDATADIR}\") # is it used anywhere in code?

install(TARGETS kdeneur DESTINATION bin)
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kdeneur/i18n)

