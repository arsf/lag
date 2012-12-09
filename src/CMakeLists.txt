SET(LAG_VERSION_MAJOR "1")
SET(LAG_VERSION_MINOR "1")
SET(LAG_VERSION_PATCH "1")
set(VERSION "${LAG_VERSION_MAJOR}.${LAG_VERSION_MINOR}.${LAG_VERSION_PATCH}")

SET(LAG_SRC_DIR  "${CMAKE_CURRENT_SOURCE_DIR}")

SET(lag_UI_SOURCES   ${LAG_SRC_DIR}/ui/AdvancedOptionsWindow.cpp
                     ${LAG_SRC_DIR}/ui/FileOpener.cpp
                     ${LAG_SRC_DIR}/ui/FileSaver.cpp
                     ${LAG_SRC_DIR}/ui/TwoDeeOverviewWindow.cpp
                     ${LAG_SRC_DIR}/ui/ProfileWindow.cpp
                     ${LAG_SRC_DIR}/ui/AdvancedLoadDialog.cpp)

SET(lag_SOURCES  ${LAG_SRC_DIR}/lag.cpp 
                 ${LAG_SRC_DIR}/MathFuncs.cpp 
                 ${LAG_SRC_DIR}/TwoDeeOverview.cpp
                 ${LAG_SRC_DIR}/BoxOverlay.cpp 
                 ${LAG_SRC_DIR}/Profile.cpp
                 ${LAG_SRC_DIR}/LagDisplay.cpp 
                 ${LAG_SRC_DIR}/Colour.cpp 
                 ${LAG_SRC_DIR}/SelectionBox.cpp 
                 ${LAG_SRC_DIR}/LoadWorker.cpp 
                 ${LAG_SRC_DIR}/SaveWorker.cpp
                 ${LAG_SRC_DIR}/ProfileWorker.cpp 
                 ${LAG_SRC_DIR}/ClassifyWorker.cpp 
                 ${LAG_SRC_DIR}/Worker.h 
                 ${LAG_SRC_DIR}/FileUtils.cpp
                 ${LAG_SRC_DIR}/geoprojectionconverter.cpp
                 ${LAG_SRC_DIR}/DrawWorker.cpp )


include_directories( ${LAG_SRC_DIR} ${LAG_SRC_DIR}/ui)
include_directories( ${CMAKE_SOURCE_DIR}/lidarquadtree/src)

include_directories( ${LASTOOLS_INCLUDE_DIRS})
include_directories( ${GTK2_INCLUDE_DIRS})
include_directories( ${GTKGLEXTMM_INCLUDE_DIRS})
include_directories( ${GTHREAD2_INCLUDE_DIRS})

add_executable(lag ${lag_UI_SOURCES} ${lag_SOURCES} )
target_link_libraries(lag ${GTK2_LIBRARIES}
                          ${GTKGLEXTMM_LIBRARIES}
                          ${GTHREAD2_LIBRARIES}
                          ${LASTOOLS_LIBRARIES}
                          lidarquatree)
set_target_properties(lag PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_custom_command(TARGET lag POST_BUILD
                              COMMAND ${CMAKE_COMMAND} -E copy 
                                        ${CMAKE_SOURCE_DIR}/lag.ui 
                                        ${CMAKE_BINARY_DIR}/bin/lag.ui)
add_custom_command(TARGET lag POST_BUILD
                              COMMAND ${CMAKE_COMMAND} -E copy_directory 
                                       ${CMAKE_SOURCE_DIR}/icons 
                                       ${CMAKE_BINARY_DIR}/bin/icons)
add_custom_command(TARGET lag POST_BUILD
                              COMMAND ${CMAKE_COMMAND} -E copy_directory 
                                       ${CMAKE_SOURCE_DIR}/lagfonts 
                                       ${CMAKE_BINARY_DIR}/bin/lagfonts)
#lag_CPPFLAGS = @GTKmm_CFLAGS@ @GTKGLextmm_CFLAGS@ @GThread_CFLAGS@ @laslib_CFLAGS@ @lidarquadtree_CFLAGS@
#lag_LDFLAGS =  @GTKmm_LIBS@ @GTKGLextmm_LIBS@ @GThread_LIBS@ @laslib_LIBS@ @lidarquadtree_LIBS@ -ljemalloc #-lboost_system
