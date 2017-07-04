set(SOURCE_EXE main.c)

include_directories(../../xneur/lib) # location of included header files

add_executable(xneurchecker ${SOURCE_EXE})
target_compile_definitions(xneurchecker PRIVATE VERSION=\"${PROJECT_VERSION}\") # VERSION definition is used in code

target_link_libraries(xneurchecker xneur) # Linking exe with library
install(TARGETS xneurchecker DESTINATION bin)
