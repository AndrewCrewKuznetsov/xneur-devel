include_directories(../ai)
include_directories(../config)
include_directories(../lib)
include_directories(../main)
include_directories(../misc)
include_directories(../notify)
include_directories(/usr/include/X11)
include_directories(/usr/include/enchant)

set(xneur_SRCS xneurlib.c)

add_library(xnlib SHARED ${xneur_SRCS})
set_target_properties(xnlib PROPERTIES VERSION 20.0.0 SOVERSION 20)
set_target_properties(xnlib PROPERTIES OUTPUT_NAME xneur) # Handle lib and exe with same names

# why libxneur.so built with cmake is different size from built with autotools?

install(FILES xneur.h DESTINATION include/xneur)
install(TARGETS xnlib DESTINATION lib)

target_link_libraries(xnlib X11 xnconfig)
