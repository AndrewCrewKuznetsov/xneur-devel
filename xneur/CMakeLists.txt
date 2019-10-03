cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0048 NEW)
project(xneur VERSION 0.20.0)

option(SOUNDS "Compile with sound support" ON) # ON|OFF|gstreamer|openal|aplay
option(CMAKE_BUILD_TYPE "Compile release or debug version" Release) # Release, Debug
option(KEYLOGGER "Compile with keylogger function" ON)
option(SPELL "Compile with spell checking function" ON) # ON|OFF|enchant|aspell both? anything_else?
# option(XOSD "Compile with xosd support" OFF) # remove xosd from project completely?
option(NOTIFICATIONS "Compile with libnotify message support" ON)
option(PLUGINS "Compile with plugins support" ON)

add_subdirectory(etc)
add_subdirectory(lib)
add_subdirectory(pixmaps)
add_subdirectory(plugins)
add_subdirectory(po)
add_subdirectory(share)
add_subdirectory(src)

install(FILES xneur.1 DESTINATION ${MAN_INSTALL_DIR}/man1)
install(FILES xneurrc.5 DESTINATION ${MAN_INSTALL_DIR}/man5)
