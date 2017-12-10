# include_directories(../ai)
# include_directories(../config)
# include_directories(../lib)
# include_directories(../main)
# include_directories(../misc)
# include_directories(../notify)
# include_directories(/usr/include/X11)

# set(xnmisc_SRCS
#     archiver.c
#     debug.c
#     list_char.c
#     log.c
#     mail.c
#     text.c
#     regexp.c)

# add_library(xnmisc STATIC ${xnmisc_SRCS})

install(FILES list_char.h DESTINATION include/xneur)
