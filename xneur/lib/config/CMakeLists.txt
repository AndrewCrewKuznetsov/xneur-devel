include_directories(../ai)
include_directories(../config)
include_directories(../lib)
include_directories(../main)
include_directories(../misc)
include_directories(../notify)
include_directories(/usr/include/X11)

set(xnconfig_SRCS xnconfig.c xnconfig_files.c
    ../lib/xneurlib.c
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


add_library(xnconfig SHARED ${xnconfig_SRCS})
target_compile_definitions(xnconfig PRIVATE VERSION=\"${PROJECT_VERSION}\") # VERSION definition is used in code
target_compile_definitions(xnconfig PRIVATE PACKAGE_SYSCONFDIR_DIR=\"/etc/xneur\") # PACKAGE_SYSCONFDIR_DIR definition is used in code
target_compile_definitions(xnconfig PRIVATE PACKAGE_SHAREDIR_DIR=\"/usr/share/xneur\") # PACKAGE_SYSCONFDIR_DIR definition is used in code
set_target_properties(xnconfig PROPERTIES VERSION 20.0.0 SOVERSION 20)

# target_compile_definitions(xnconfig PRIVATE VERSION=\"${PROJECT_VERSION}\") # seems like VERSION definition is not used in code
target_compile_definitions(xnconfig PRIVATE PACKAGE=\"${PROJECT_NAME}\") # PACKAGE definition is used in code

target_link_libraries(xnconfig Xtst)

install(FILES xnconfig.h DESTINATION include/xneur)
install(TARGETS xnconfig DESTINATION lib)
