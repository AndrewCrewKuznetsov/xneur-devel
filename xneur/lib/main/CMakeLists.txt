include_directories(../ai)
include_directories(../config)
include_directories(../lib)
include_directories(../main)
include_directories(../misc)
include_directories(../notify)

include_directories(/usr/include)
include_directories(/usr/include/X11)
include_directories(/usr/include/X11/extensions)

set(xnmain_SRCS
    bind_table.c
    buffer.c
    event.c
    focus.c
    keymap.c
    plugin.c
    program.c
    selection.c
    switchlang.c
    utils.c
    window.c
    )

add_library(xnmain STATIC ${xnmain_SRCS})
target_compile_definitions(xnmain PRIVATE VERSION=\"${PROJECT_VERSION}\") # VERSION definition is used in code
target_compile_definitions(xnmain PRIVATE PACKAGE=\"${PROJECT_NAME}\") # PACKAGE definition is used in code
