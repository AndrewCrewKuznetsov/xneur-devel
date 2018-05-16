if(PLUGINS)
	set(xntest_SRCS test.c)
	add_library(xntest SHARED ${xntest_SRCS})
	# add_library(xntest MODULE ${xntest_SRCS}) # Do we need use MODULE instead of SHARED? If use MODULE, then so symlinks are not created
	set_target_properties(xntest PROPERTIES VERSION 0.0.0 SOVERSION 0)
	target_compile_definitions(xntest PRIVATE WITH_PLUGINS=1)
	target_include_directories(xntest PUBLIC "${CMAKE_SOURCE_DIR}/lib/config")
	target_include_directories(xntest PUBLIC "${CMAKE_SOURCE_DIR}/lib/lib")
	target_compile_definitions(xntest PRIVATE PACKAGE_SYSCONFDIR_DIR=\"/etc/xneur\") # needed?
	target_compile_definitions(xntest PRIVATE PACKAGE_SHAREDIR_DIR=\"/usr/share/xneur\") # needed?
	target_link_libraries(xntest X11 Xtst Xi xnconfig xnlib) # do we need to link with xnconfig and xnlib?
	install(TARGETS xntest LIBRARY DESTINATION lib/xneur)
endif(PLUGINS)
