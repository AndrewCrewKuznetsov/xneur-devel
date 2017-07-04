cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0048 NEW)
project(xneur VERSION 0.20.0)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR})

add_subdirectory(etc)
add_subdirectory(lib)
add_subdirectory(pixmaps)
add_subdirectory(plugins)
add_subdirectory(po)
add_subdirectory(share)
add_subdirectory(src)

find_package(ECM 5.18.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs) # For gzipping mans
install(FILES xneur.1 DESTINATION ${MAN_INSTALL_DIR}/man1)
install(FILES xneurrc.5 DESTINATION ${MAN_INSTALL_DIR}/man5)
