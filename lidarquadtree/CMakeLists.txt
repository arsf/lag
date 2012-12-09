###############################################################################
# Main CMake configuration file for libLidarQuadtree
#
# Author: Nicolas David <nicolas.david@ign.fr>
#
#
###############################################################################
# liblidarquadtree

SET(LIBLIDARQUADTREE_VERSION_MAJOR "1")
SET(LIBLIDARQUADTREE_VERSION_MINOR "3")
SET(LIBLIDARQUADTREE_VERSION_PATCH "0")
SET(VERSION
"${LIBLIDARQUADTREE_VERSION_MAJOR}.${LIBLIDARQUADTREE_VERSION_MINOR}.${LIBLIDARQUADTREE_VERSION_PATCH}")

SET(LIBQUADTREE_SRC_DIR "src")
SET(LIBLIDARQUADTREE_SRC ${LIBQUADTREE_SRC_DIR}/CollisionDetection.cpp
                         ${LIBQUADTREE_SRC_DIR}/LidarPoint.cpp 
                         ${LIBQUADTREE_SRC_DIR}/Point.cpp 
                         ${LIBQUADTREE_SRC_DIR}/PointBucket.cpp 
                         ${LIBQUADTREE_SRC_DIR}/QuadtreeNode.cpp 
                         ${LIBQUADTREE_SRC_DIR}/CacheMinder.cpp 
                         ${LIBQUADTREE_SRC_DIR}/Quadtree.cpp 
                         ${LIBQUADTREE_SRC_DIR}/PointData.h)

SET(LIBLIDARQUADTREE_HEADERS ${LIBQUADTREE_SRC_DIR}/Boundary.h
                             ${LIBQUADTREE_SRC_DIR}/CacheMinder.h
                             ${LIBQUADTREE_SRC_DIR}/CollisionDetection.h
                             ${LIBQUADTREE_SRC_DIR}/LidarPoint.h
                             ${LIBQUADTREE_SRC_DIR}/PointBucket.h
                             ${LIBQUADTREE_SRC_DIR}/Point.h
                             ${LIBQUADTREE_SRC_DIR}/QuadtreeExceptions.h
                             ${LIBQUADTREE_SRC_DIR}/Quadtree.h
                             ${LIBQUADTREE_SRC_DIR}/QuadtreeNode.h
                             ${LIBQUADTREE_SRC_DIR}/PointData.h)
                             
include_directories( ${LIBQUADTREE_SRC_DIR})
include_directories( ${LASTOOLS_INCLUDE_DIRS})
include_directories( ${LZO_INCLUDE_DIR})

add_library(lidarquatree STATIC ${LIBLIDARQUADTREE_SRC} ${LIBLIDARQUADTREE_HEADERS} )
target_link_libraries(lidarquatree ${LASTOOLS_LIBRARIES} ${LZO_LIBRARIES})

