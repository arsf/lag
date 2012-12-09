###############################################################################
# Main CMake configuration file for LAG
#
# Author: Nicolas David <nicolas.david@ign.fr>
#
#
###############################################################################
# LAG general settings
project(LAG)
cmake_minimum_required(VERSION 2.8.0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
message(STATUS "CMAKE_LIBRARY_ARCHITECTURE ${CMAKE_LIBRARY_ARCHITECTURE}")
message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")

find_package(GTK2 2.6 REQUIRED gtkmm)
if(NOT GTK2_FOUND)
    message(SEND_ERROR "gtkmm not found")
endif()

find_package(GTHREAD2  REQUIRED)
if(NOT GTHREAD2_FOUND)
    message(SEND_ERROR "gthread-2. not found")
else()
    message(STATUS "GTHREAD2_INCLUDE_DIRS = ${GTHREAD2_INCLUDE_DIRS}")
    message(STATUS "GTHREAD2_LIBRARIES = ${GTHREAD2_LIBRARIES}")
endif()

find_package(GTKGLEXTMM  REQUIRED)
if(NOT GTKGLEXTMM_FOUND)
    message(SEND_ERROR "gtkglextmm-1.2 not found")
endif()

find_package(LASTOOLS  REQUIRED)
if(NOT LASTOOLS_FOUND)
    message(SEND_ERROR "lastools not found, set LASTOOLS_ROOT to the root\ 
    directory of the lastools install path" )
endif()

find_package(LZO  REQUIRED)
if(NOT LZO_FOUND)
    message(SEND_ERROR "lzo2 not found" )
endif()

add_subdirectory(lidarquadtree)
add_subdirectory(src)

add_custom_command(TARGET all PRE_BUILD
                              COMMAND ${CMAKE_COMMAND} -E copy_directory
                                        ${LASTOOLS_INCLUDE_DIRS} 
                                        ${LASTOOLS_INCLUDE_DIRS}/laslib)
