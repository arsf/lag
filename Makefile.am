# Makefile.am for LiDAR Analysis GUI

AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = lag
#AM_CXXFLAGS=-O4
lag_SOURCES = src/lag.cpp src/AdvancedOptionsWindow.cpp src/FileOpener.cpp src/MathFuncs.cpp src/TwoDeeOverview.cpp src/BoxOverlay.cpp src/FileSaver.cpp src/Profile.cpp src/TwoDeeOverviewWindow.cpp src/Display.cpp src/ProfileWindow.cpp
AM_CPPFLAGS = -I./quadtreelink/src `pkg-config --cflags gtkmm-2.4 libglademm-2.4 gtkglextmm-1.2 gthread-2.0` 
AM_LDFLAGS = -L./quadtreelink/.libs `pkg-config --libs gtkmm-2.4 libglademm-2.4 gtkglextmm-1.2 gthread-2.0` -llidarquadtree -llas -lGLC 
