cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0048 NEW)
project(gxneur VERSION 0.20.0)

option(CMAKE_BUILD_TYPE "Compile release or debug version" Release) # Release, Debug
option(APPINDICATOR "Compile with AppIndicator library support" ON)
option(GCONF "Compile with GConf support" ON)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}) # ?

add_subdirectory(pixmaps)
add_subdirectory(po)
add_subdirectory(src)
add_subdirectory(ui)

find_package(ECM 5.18.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

install(FILES gxneur.desktop DESTINATION /usr/share/applications)

include(KDEInstallDirs) # For gzipping mans # we should not use kde stuff in gnome programm
install(FILES gxneur.1 DESTINATION ${MAN_INSTALL_DIR}/man1)
