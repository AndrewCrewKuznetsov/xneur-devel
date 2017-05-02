add_subdirectory(notify)
add_subdirectory(misc)
add_subdirectory(main)
add_subdirectory(ai)
add_subdirectory(lib)
add_subdirectory(config)

# Making pkgconfig .pc files from pc.in files
# xnconfig.pc
set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "\${prefix}")
set(libdir "\${exec_prefix}/lib")
set(includedir "\${prefix}/include")
set(VERSION "${PROJECT_VERSION}")
set(PCRE_LIBS "-lpcre -Wl,-O1,--sort-common,--as-needed,-z,relro")
set(LDFLAGS "-lpthread")
set(ADDITIONAL_LIBS "-ldl")

configure_file(../xnconfig.pc.in xnconfig.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xnconfig.pc DESTINATION lib/pkgconfig)

# xneur.pc
set(ENCHANT_LIBS "-lenchant")
set(PCRE_LIBS "-lpcre")
set(ADDITIONAL_LIBS "-lpthread -ldl")

configure_file(../xneur.pc.in xneur.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xneur.pc DESTINATION lib/pkgconfig)
