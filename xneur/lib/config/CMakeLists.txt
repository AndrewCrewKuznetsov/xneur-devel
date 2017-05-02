include_directories(../ai)
include_directories(../config)
include_directories(../lib)
include_directories(../main)
include_directories(../misc)
include_directories(../notify)
include_directories(/usr/include/X11)

set(xnconfig_SRCS xnconfig.c xnconfig_files.c)

add_library(xnconfig SHARED ${xnconfig_SRCS})
target_compile_definitions(xnconfig PRIVATE VERSION=\"${PROJECT_VERSION}\") # VERSION definition is used in code
target_compile_definitions(xnconfig PRIVATE PACKAGE_SYSCONFDIR_DIR=\"/etc/xneur\") # PACKAGE_SYSCONFDIR_DIR definition is used in code
target_compile_definitions(xnconfig PRIVATE PACKAGE_SHAREDIR_DIR=\"/usr/share/xneur\") # PACKAGE_SYSCONFDIR_DIR definition is used in code
set_target_properties(xnconfig PROPERTIES VERSION 20.0.0 SOVERSION 20)

install(FILES xnconfig.h DESTINATION include/xneur)
install(TARGETS xnconfig DESTINATION lib)
