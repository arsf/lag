# - Try to find GTKGLEXTMM 
# Find GThread headers, libraries and the answer to all questions.
#
#  GTKGLEXTMM_FOUND               True if GTKGLEXTMM got found
#  GTKGLEXTMM_INCLUDE_DIRS        Location of GTKGLEXTMM headers 
#  GTKGLEXTMM_LIBRARIES           List of libraries to use GTKGLEXTMM 
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

INCLUDE(FindPkgConfig )
INCLUDE(FindPackageHandleStandardArgs)

IF (GTKGLEXTMM_FIND_REQUIRED )
    SET( _pkgconfig_REQUIRED "REQUIRED" )
ELSE(GTKGLEXTMM_FIND_REQUIRED )
    SET( _pkgconfig_REQUIRED "" )
ENDIF(GTKGLEXTMM_FIND_REQUIRED )


message(STATUS "_pkgconfig_REQUIRED ${_pkgconfig_REQUIRED}")

IF (GTKGLEXTMM_MIN_VERSION )
    PKG_SEARCH_MODULE(GTKGLEXTMM ${_pkgconfig_REQUIRED} gtkglextmm-1.2>=${GTKGLEXTMM_MIN_VERSION} )
ELSE (GTKGLEXTMM_MIN_VERSION )
    PKG_SEARCH_MODULE(GTKGLEXTMM ${_pkgconfig_REQUIRED} gtkglextmm-1.2 )
    #message(STATUS "GTKGLEXTMM_FOUND  ${GTKGLEXTMM_FOUND}")
    #message(STATUS "GTKGLEXTMM_LIBRARIES  ${GTKGLEXTMM_LIBRARIES}")
    #message(STATUS "GTKGLEXTMM_INCLUDE_DIRS  ${GTKGLEXTMM_INCLUDE_DIRS}")
ENDIF (GTKGLEXTMM_MIN_VERSION )



IF(NOT GTKGLEXTMM_FOUND AND NOT PKG_CONFIG_FOUND )
    message(SEND_ERROR "PKG_CONFIG_FOUND ${PKG_CONFIG_FOUND}")
ENDIF(NOT GTKGLEXTMM_FOUND AND NOT PKG_CONFIG_FOUND )

#pkg_check_modules(PC_GTKGLEXTMM REQUIRED gtkglextmm-1.2)
#set(GTKGLEXTMM_DEFINITIONS ${PC_GTKGLEXTMM_CFLAGS_OTHER})

#find_path(GTKGLEXTMM_INCLUDE_DIR gtkglmm.h
#        HINTS ${PC_GTKGLEXTMM_INCLUDEDIR} ${PC_GTKGLEXTMM_INCLUDE_DIRS}
#        PATH_SUFFIXES gtkglextmm-1.2)

#find_library(GTKGLEXTMM_LIBRARY NAMES libgtkglextmm-1.2 gtkglextmm-1.2 
#        HINTS ${PC_GTKGLEXTMM_LIBDIR} ${PC_GTKGLEXTMM_LIBRARY_DIRS})
        

find_package_handle_standard_args(gtkglextmm DEFAULT_MSG
        GTKGLEXTMM_LIBRARIES GTKGLEXTMM_INCLUDE_DIRS)
        
mark_as_advanced(GTKGLEXTMM_INCLUDE_DIRS GTKGLEXTMM_LIBRARIES)

