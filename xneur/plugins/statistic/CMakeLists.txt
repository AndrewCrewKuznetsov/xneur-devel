set(xnstatistic_SRCS statistic.c)

add_library(xnstatistic SHARED ${xnstatistic_SRCS})
set_target_properties(xnstatistic PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS xnstatistic LIBRARY DESTINATION lib/xneur)

# why libxnstatistic.so built with cmake is different size from built with autotools?
