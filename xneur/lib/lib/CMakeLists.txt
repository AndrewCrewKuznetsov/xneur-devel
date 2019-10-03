include_directories(../ai)
include_directories(../config)
include_directories(../lib)
include_directories(../main)
include_directories(../misc)
include_directories(../notify)
include_directories(/usr/include/X11)
include_directories(/usr/include/enchant)

set(xneur_SRCS
	xneurlib.c 
	../misc/archiver.c
    ../misc/debug.c
    ../misc/list_char.c
    ../misc/log.c
    ../misc/mail.c
    ../misc/text.c
    ../misc/regexp.c
    ../main/bind_table.c
    ../main/buffer.c
    ../main/event.c
    ../main/focus.c
    ../main/keymap.c
    ../main/plugin.c
    ../main/program.c
    ../main/selection.c
    ../main/switchlang.c
    ../main/utils.c
    ../main/window.c
    ../ai/conversion.c
    ../ai/detection.c
    ../notify/notify.c
    ../notify/osd.c
    ../notify/popup.c
    ../notify/sound.c
    )

add_library(xnlib SHARED ${xneur_SRCS})

if(SOUNDS)
	target_compile_definitions(xnlib PRIVATE WITH_SOUND=1)

	if("${SOUNDS}" STREQUAL "ON")
		set (SOUNDS "gstreamer") # default is gstreamer
	endif()

	if("${SOUNDS}" STREQUAL "gstreamer")
		find_package(PkgConfig)
		pkg_search_module(GST REQUIRED gstreamer-1.0>=1.4 gstreamer-0.10>=0.10.6)
		include_directories(${GST_INCLUDE_DIRS})
		target_compile_definitions(xnlib PRIVATE WITH_GSTREAMER=1)
	elseif("${SOUNDS}" STREQUAL "openal")
		find_package(PkgConfig)
		pkg_check_modules(FREEALUT REQUIRED freealut>=1.0.1 )
		include_directories(${FREEALUT_INCLUDE_DIRS})
		target_compile_definitions(xnlib PRIVATE WITH_OPENAL=1)
	elseif("${SOUNDS}" STREQUAL "aplay")
		set (APLAY_LIBS "-lpthread")
		target_compile_definitions(xnlib PRIVATE WITH_APLAY=1)
	else()
		message (FATAL_ERROR "Unsupported sound system selected. Turn SOUNDS off or choose one of the following:\n gstreamer (default), openal, aplay.")
	endif()
endif(SOUNDS)

if(KEYLOGGER)
	target_compile_definitions(xnlib PRIVATE WITH_KEYLOGGER=1)
endif(KEYLOGGER)

if(SPELL)
	if("${SPELL}" STREQUAL "ON")
		set (SPELL "enchant") # default is enchant
	endif()
	# Should we handle case when both enabled?
	if("${SPELL}" STREQUAL "enchant")
		find_package(PkgConfig)
		pkg_check_modules(ENCHANT REQUIRED enchant>=1.0)
		include_directories(${ENCHANT_INCLUDE_DIRS})
		target_compile_definitions(xnlib PRIVATE WITH_ENCHANT=1)
	elseif("${SPELL}" STREQUAL "aspell")
		include_directories("/usr/include/") # aspell.h
		set(ASPELL_LIBS "aspell") # "-L/usr/lib -laspell"
		target_compile_definitions(xnlib PRIVATE WITH_ASPELL=1)
	else()
		message (FATAL_ERROR "Unsupported spell system selected. Turn SPELL off or choose one or both of the following:\n enchant (default), aspell.")
	endif()
endif(SPELL)

# if(XOSD)
# 	execute_process(COMMAND xosd-config --libs OUTPUT_VARIABLE XOSD_LIBS)
# 	XOSD_LIBS=`xosd-config --libs`
# 	set(XOSD_LIBS "-L/usr/lib -lxosd -lpthread -lXext -lX11 -lXinerama")
# message ( ======= XOSD_LIBS = "#${XOSD_LIBS}#")
# 	target_compile_definitions(xnlib PRIVATE WITH_XOSD=1)
# endif(XOSD)

if(NOTIFICATIONS)
	find_package(PkgConfig)
	pkg_check_modules(LIBNOTIFY REQUIRED libnotify>=0.4.0)
	include_directories(${LIBNOTIFY_INCLUDE_DIRS})
	target_compile_definitions(xnlib PRIVATE WITH_LIBNOTIFY=1)
endif(NOTIFICATIONS)

if(PLUGINS)
	target_compile_definitions(xnlib PRIVATE WITH_PLUGINS=1)
	target_compile_definitions(xnlib PRIVATE XNEUR_PLUGIN_DIR="/usr/lib/xneur")
endif(PLUGINS)

set_target_properties(xnlib PROPERTIES VERSION 20.0.0 SOVERSION 20)
set_target_properties(xnlib PROPERTIES OUTPUT_NAME xneur) # Handle lib and exe with same names
target_compile_definitions(xnlib PRIVATE VERSION=\"${PROJECT_VERSION}\") # VERSION definition is used in code
target_compile_definitions(xnlib PRIVATE PACKAGE=\"${PROJECT_NAME}\") # PACKAGE definition is used in code

# why libxneur.so built with cmake is different size from built with autotools?

install(FILES xneur.h DESTINATION include/xneur)
install(TARGETS xnlib DESTINATION lib)

target_link_libraries(xnlib X11 Xtst Xi xnconfig pcre z ${GST_LIBRARIES} ${LIBNOTIFY_LIBRARIES} ${XOSD_LIBS} ${FREEALUT_LIBRARIES} ${APLAY_LIBS} ${ASPELL_LIBS} ${ENCHANT_LIBRARIES}) # added Xtst and Xi remove ${XOSD_LIBS} ?
