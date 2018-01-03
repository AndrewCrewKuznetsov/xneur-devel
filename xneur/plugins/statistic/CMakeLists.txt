if(PLUGINS)
	set(xnstatistic_SRCS statistic.c)
	add_library(xnstatistic SHARED ${xnstatistic_SRCS})
	# add_library(xnstatistic MODULE ${xnstatistic_SRCS}) # Do we need use MODULE instead of SHARED? If use MODULE, then so symlinks are not created
	set_target_properties(xnstatistic PROPERTIES VERSION 0.0.0 SOVERSION 0)
	target_compile_definitions(xnstatistic PRIVATE WITH_PLUGINS=1)
	target_include_directories(xnstatistic PUBLIC "${CMAKE_SOURCE_DIR}/lib/config")
	target_include_directories(xnstatistic PUBLIC "${CMAKE_SOURCE_DIR}/lib/lib")
	target_compile_definitions(xntest PRIVATE PACKAGE_SYSCONFDIR_DIR=\"/etc/xneur\") # needed?
	target_compile_definitions(xntest PRIVATE PACKAGE_SHAREDIR_DIR=\"/usr/share/xneur\") # needed?
	target_link_libraries(xnstatistic X11 Xtst Xi xnconfig xnlib) # do we need to link with xnconfig and xnlib?
	install(TARGETS xnstatistic LIBRARY DESTINATION lib/xneur)
endif(PLUGINS)

# why libxnstatistic.so built with cmake is different size from built with autotools?
