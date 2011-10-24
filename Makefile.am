# Makefile.am for LiDAR Analysis GUI

AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = lag
AM_CXXFLAGS=-Wall -Wextra -Werror -DLINUX -g -DDEBUG_ALL -ggdb
# allow automake to find any required extra m4 macros
ACLOCAL_AMFLAGS = -I m4

lag_SOURCES = src/lag.cpp src/ui/AdvancedOptionsWindow.cpp \
src/ui/FileOpener.cpp src/MathFuncs.cpp src/TwoDeeOverview.cpp \
src/BoxOverlay.cpp src/ui/FileSaver.cpp src/Profile.cpp \
src/ui/TwoDeeOverviewWindow.cpp src/LagDisplay.cpp src/ui/ProfileWindow.cpp \
src/Colour.cpp src/SelectionBox.cpp 
lag_CPPFLAGS = $(DEPS_CFLAGS)
lag_LDADD = $(DEPS_LIBS) $(LDFLAGS)

