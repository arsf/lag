# Makefile.am for LiDAR Analysis GUI

AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = lag
#AM_CXXFLAGS=-O4
lag_SOURCES = src/lag.cpp src/ui/AdvancedOptionsWindow.cpp src/ui/FileOpener.cpp src/MathFuncs.cpp src/TwoDeeOverview.cpp src/BoxOverlay.cpp src/ui/FileSaver.cpp src/Profile.cpp src/ui/TwoDeeOverviewWindow.cpp src/LagDisplay.cpp src/ui/ProfileWindow.cpp src/Colour.cpp src/SelectionBox.cpp
AM_CPPFLAGS = -I./quadtreelink/src `pkg-config --cflags gtkmm-2.4 gtkglextmm-1.2 gthread-2.0` -I /usr/include/freetype2 
#AM_LDFLAGS = -L./quadtreelink/.libs `pkg-config --libs gtkmm-2.4 gtkglextmm-1.2 gthread-2.0` -llidarquadtree -llas -lGLC -lglut 
AM_LDFLAGS = -L./quadtreelink/.libs -pthread -Wl,--export-dynamic -lgtkglextmm-x11-1.2 -lgdkglextmm-x11-1.2 -lgtkglext-x11-1.0 -lgtkmm-2.4 -lgdkglext-x11-1.0 -lGLU -lGL -lXmu -lXt -lSM -lICE -lpangox-1.0 -lX11 -latkmm-1.6 -lgdkmm-2.4 -lgiomm-2.4 -lpangomm-1.4 -lgtk-x11-2.0 -lglibmm-2.4 -lcairomm-1.0 -lsigc-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lgdk_pixbuf-2.0 -lpangocairo-1.0 -lcairo -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0   -llidarquadtree -llas -lftgl
