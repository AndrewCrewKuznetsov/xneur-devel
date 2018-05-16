cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0048 NEW)
project(kdeneur VERSION 0.20.0)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR})
find_package(KDE4 REQUIRED)
find_package(Qt4 REQUIRED QtDBus QtGui QtCore)

add_subdirectory(pixmaps)
set (TR_FILES) # Should be before add_subdirectory(i18n). Used for transferring var between several CmakeLists.txt
add_subdirectory(i18n) # Shoul be before src, because TR_FILES is setting there
add_subdirectory(src)

find_package(ECM 5.18.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

install(FILES kdeneur.desktop DESTINATION /usr/share/applications)

include(KDEInstallDirs) # For gzipping mans
install(FILES kdeneur.1 DESTINATION ${MAN_INSTALL_DIR}/man1)
