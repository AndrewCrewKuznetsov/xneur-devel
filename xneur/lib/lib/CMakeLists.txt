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
set_target_properties(xnlib PROPERTIES VERSION 20.0.0 SOVERSION 20)
set_target_properties(xnlib PROPERTIES OUTPUT_NAME xneur) # Handle lib and exe with same names
target_compile_definitions(xnlib PRIVATE VERSION=\"${PROJECT_VERSION}\") # VERSION definition is used in code
target_compile_definitions(xnlib PRIVATE PACKAGE=\"${PROJECT_NAME}\") # PACKAGE definition is used in code

# why libxneur.so built with cmake is different size from built with autotools?

install(FILES xneur.h DESTINATION include/xneur)
install(TARGETS xnlib DESTINATION lib)

target_link_libraries(xnlib X11 Xtst Xi xnconfig pcre z) # added Xtst and Xi
