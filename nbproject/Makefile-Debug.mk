#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include lag-Makefile.mk

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/MathFuncs.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/QuadtreeNode.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/lag.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/ProfileWindow.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/quadtreestructs.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/TwoDeeOverview.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/LasSaver.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/Quadtree.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/AsciiLoader.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/PointBucket.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/commonfunctions.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/AdvancedOptionsWindow.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/BoxOverlay.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/CollisionDetection.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/Profile.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/Display.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/FileSaver.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/TwoDeeOverviewWindow.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/LasLoader.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/CacheMinder.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/FileOpener.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-I /users/rsg/chrfi/liblas_x64/include `pkg-config --cflags --libs gtkmm-2.4 libglademm-2.4 cairomm-1.0 gtkglextmm-1.2 gthread-2.0` -g -pg  -Wall   -O4 
CXXFLAGS=-I /users/rsg/chrfi/liblas_x64/include `pkg-config --cflags --libs gtkmm-2.4 libglademm-2.4 cairomm-1.0 gtkglextmm-1.2 gthread-2.0` -g -pg  -Wall   -O4 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/users/rsg/chrfi/newsvn/lag/TCMalloc/lib -lboost_iostreams-mt -lboost_filesystem-mt -llas -lboost_thread-mt -lboost_serialization-mt -llzo2 /users/rsg/chrfi/git_repos/lag/TCMalloc/lib/libtcmalloc.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/lag

dist/Debug/GNU-Linux-x86/lag: /users/rsg/chrfi/git_repos/lag/TCMalloc/lib/libtcmalloc.a

dist/Debug/GNU-Linux-x86/lag: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lag ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/MathFuncs.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/MathFuncs.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/MathFuncs.o /users/rsg/chrfi/git_repos/lag/MathFuncs.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/QuadtreeNode.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/QuadtreeNode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/QuadtreeNode.o /users/rsg/chrfi/git_repos/lag/QuadtreeNode.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/lag.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/lag.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/lag.o /users/rsg/chrfi/git_repos/lag/lag.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/ProfileWindow.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/ProfileWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/ProfileWindow.o /users/rsg/chrfi/git_repos/lag/ProfileWindow.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/quadtreestructs.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/quadtreestructs.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/quadtreestructs.o /users/rsg/chrfi/git_repos/lag/quadtreestructs.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/TwoDeeOverview.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/TwoDeeOverview.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/TwoDeeOverview.o /users/rsg/chrfi/git_repos/lag/TwoDeeOverview.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/LasSaver.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/LasSaver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/LasSaver.o /users/rsg/chrfi/git_repos/lag/LasSaver.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/Quadtree.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/Quadtree.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/Quadtree.o /users/rsg/chrfi/git_repos/lag/Quadtree.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/AsciiLoader.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/AsciiLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/AsciiLoader.o /users/rsg/chrfi/git_repos/lag/AsciiLoader.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/PointBucket.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/PointBucket.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/PointBucket.o /users/rsg/chrfi/git_repos/lag/PointBucket.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/commonfunctions.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/commonfunctions.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/commonfunctions.o /users/rsg/chrfi/git_repos/lag/commonfunctions.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/AdvancedOptionsWindow.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/AdvancedOptionsWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/AdvancedOptionsWindow.o /users/rsg/chrfi/git_repos/lag/AdvancedOptionsWindow.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/BoxOverlay.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/BoxOverlay.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/BoxOverlay.o /users/rsg/chrfi/git_repos/lag/BoxOverlay.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/CollisionDetection.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/CollisionDetection.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/CollisionDetection.o /users/rsg/chrfi/git_repos/lag/CollisionDetection.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/Profile.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/Profile.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/Profile.o /users/rsg/chrfi/git_repos/lag/Profile.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/Display.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/Display.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/Display.o /users/rsg/chrfi/git_repos/lag/Display.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/FileSaver.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/FileSaver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/FileSaver.o /users/rsg/chrfi/git_repos/lag/FileSaver.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/TwoDeeOverviewWindow.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/TwoDeeOverviewWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/TwoDeeOverviewWindow.o /users/rsg/chrfi/git_repos/lag/TwoDeeOverviewWindow.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/LasLoader.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/LasLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/LasLoader.o /users/rsg/chrfi/git_repos/lag/LasLoader.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/CacheMinder.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/CacheMinder.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/CacheMinder.o /users/rsg/chrfi/git_repos/lag/CacheMinder.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/FileOpener.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/FileOpener.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/FileOpener.o /users/rsg/chrfi/git_repos/lag/FileOpener.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/lag

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
