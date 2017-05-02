set(xntest_SRCS test.c)

add_library(xntest SHARED ${xntest_SRCS})
set_target_properties(xntest PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS xntest LIBRARY DESTINATION lib/xneur)

# why libxntest.so built with cmake is different size from built with autotools?
