# Makefile.am for LiDAR Analysis GUI

AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = lag
AM_CXXFLAGS=-Wall -Wextra -Werror -DLINUX
# allow automake to find any required extra m4 macros
ACLOCAL_AMFLAGS = -I m4

lag_SOURCES = src/lag.cpp src/ui/AdvancedOptionsWindow.cpp \
src/ui/FileOpener.cpp src/MathFuncs.cpp src/TwoDeeOverview.cpp \
src/BoxOverlay.cpp src/ui/FileSaver.cpp src/Profile.cpp \
src/ui/TwoDeeOverviewWindow.cpp src/LagDisplay.cpp src/ui/ProfileWindow.cpp \
src/Colour.cpp src/SelectionBox.cpp src/LoadWorker.cpp src/SaveWorker.cpp \
src/ProfileWorker.cpp src/ClassifyWorker.cpp src/Worker.h
lag_CPPFLAGS = @GTKmm_CFLAGS@ @GTKGLextmm_CFLAGS@ @GThread_CFLAGS@ @libgeotiff_CFLAGS@ @lidarquadtree_CFLAGS@
lag_LDFLAGS = @GTKmm_LIBS@ @GTKGLextmm_LIBS@ @GThread_LIBS@ @libgeotiff_LIBS@ @lidarquadtree_LIBS@ -lboost_filesystem -lboost_system
lagdir = $(prefix)/share/lag
lag_DATA = lag.ui
lagiconsdir = $(prefix)/share/lag/icons
lagicons_DATA = icons/*
