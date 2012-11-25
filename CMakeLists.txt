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

set(GT2_DEBUG 1)
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

include(FindLastools.cmake)
#add_subdirectory(lidarquadtree)
#add_subdirectory(src)


