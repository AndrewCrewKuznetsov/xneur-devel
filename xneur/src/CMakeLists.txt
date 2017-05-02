find_package(X11 REQUIRED)

set(SOURCE_EXE xneur.c)
set(SOURCE_LIB newlang_creation.c)

include_directories(../lib/ai)
include_directories(../lib/config)
include_directories(../lib/lib)
include_directories(../lib/main)
include_directories(../lib/misc)
include_directories(../lib/notify)

include_directories(/usr/include/X11/extensions)

add_executable(xneur-bin ${SOURCE_EXE})
target_compile_definitions(xneur-bin PRIVATE VERSION=\"${PROJECT_VERSION}\") # VERSION definition is used in code
target_compile_definitions(xneur-bin PRIVATE PACKAGE=\"${PROJECT_NAME}\") # PACKAGE definition is used in code

add_library(newlang_creation STATIC ${SOURCE_LIB})

target_link_libraries(xneur-bin xnai newlang_creation xneur xnconfig xnmain xnmisc xnnotify X11 Xtst Xi pcre pthread)
set_target_properties(xneur-bin PROPERTIES OUTPUT_NAME xneur) # Hndle lib and exe with same names
install(TARGETS xneur-bin DESTINATION bin)

# why xneur built with cmake is different size from built with autotools?
