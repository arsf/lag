# Makefile.am for LiDAR QuadTree

ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = subdir-objects
lib_LTLIBRARIES = liblidarquadtree.la
AM_CXXFLAGS=-Wall -Wextra -DLINUX
liblidarquadtree_la_SOURCES = src/CollisionDetection.cpp src/LidarPoint.cpp src/Point.cpp src/PointBucket.cpp src/QuadtreeNode.cpp src/CacheMinder.cpp src/Quadtree.cpp src/PointData.h
liblidarquadtree_ladir= $(includedir)/lidar/quadtree
liblidarquadtree_la_HEADERS = src/Boundary.h \
src/CacheMinder.h src/CollisionDetection.h \
src/LidarPoint.h \
src/PointBucket.h src/Point.h src/QuadtreeExceptions.h src/Quadtree.h \
src/QuadtreeNode.h src/PointData.h
liblidarquadtree_la_CPPFLAGS = $(laslib_CFLAGS)
liblidarquadtree_la_LIBADD = $(laslib_LIBS)
# pkgconfig creation and installation
# see http://www.eyrie.org/~eagle/notes/style/build.html
pkgconfigdir = $(libdir)/pkgconfig
nodist_pkgconfig_DATA = lidarquadtree.pc
lidarquadtree.pc: $(srcdir)/lidarquadtree.pc.in
	sed -e 's![@]prefix[@]!$(prefix)!g' \
       -e 's![@]exec_prefix[@]!$(exec_prefix)!g' \
       -e 's![@]includedir[@]!$(includedir)!g' \
       -e 's![@]libdir[@]!$(libdir)!g' \
       -e 's![@]PACKAGE_VERSION[@]!$(PACKAGE_VERSION)!g' \
       $(srcdir)/lidarquadtree.pc.in > $@
AM_LDFLAGS = -version-info 0:1:0 -llzo2 -llaslib -ljemalloc
