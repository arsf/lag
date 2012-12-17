# Try to find lslib from lastools
# Find GThread headers, libraries and the answer to all questions.
#
#  LASTOOLS_FOUND               True if LASTOOLS got found
#  LASTOOLS_INCLUDE_DIRS        Location of LASTOOLS headers 
#  LASTOOLS_LIBRARIES           List of libraries to use LASTOOLS 
#
#  config with LASTOOLS_ROOT
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.

INCLUDE(FindPackageHandleStandardArgs)


FIND_PATH(LASTOOLS_INCLUDE_DIRS lasreader.hpp 
                           PATH ${LASTOOLS_ROOT}/laslib/inc
                                ${CMAKE_SOURCE_DIR}/lastools/laslib/inc
                                ${CMAKE_SOURCE_DIR}/../lastools/laslib/inc)

FIND_LIBRARY(LASTOOLS_LIBRARIES laslib 
                           PATH ${LASTOOLS_ROOT}/laslib/lib
                                ${CMAKE_SOURCE_DIR}/lastools/laslib/lib
                                ${CMAKE_SOURCE_DIR}/../lastools/laslib/lib)
                                
FIND_PATH(LASTOOLS_SOURCE_DIRS lasreader.cpp 
                           PATH ${LASTOOLS_ROOT}/laslib/src
                                ${CMAKE_SOURCE_DIR}/lastools/laslib/src
                                ${CMAKE_SOURCE_DIR}/../lastools/laslib/src)

find_package_handle_standard_args(LASTOOLS DEFAULT_MSG
        LASTOOLS_LIBRARIES LASTOOLS_INCLUDE_DIRS)
        
mark_as_advanced(LASTOOLS_INCLUDE_DIRS LASTOOLS_LIBRARIES)
