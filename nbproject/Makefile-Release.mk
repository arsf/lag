#
# Gererated Makefile - do not edit!
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
FC=

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/Release/GNU-Linux-x86

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/LASloader.o \
	${OBJECTDIR}/quadtreestructs.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/TwoDeeOverview.o \
	${OBJECTDIR}/quadtreenode.o \
	${OBJECTDIR}/lidar_loader.o \
	${OBJECTDIR}/quadtree.o \
	${OBJECTDIR}/test.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} dist/Release/GNU-Linux-x86/svncheckoutstuff

dist/Release/GNU-Linux-x86/svncheckoutstuff: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Linux-x86
	${LINK.cc} -o dist/Release/GNU-Linux-x86/svncheckoutstuff ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/LASloader.o: LASloader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/LASloader.o LASloader.cpp

${OBJECTDIR}/quadtreestructs.o: quadtreestructs.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/quadtreestructs.o quadtreestructs.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/TwoDeeOverview.o: TwoDeeOverview.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/TwoDeeOverview.o TwoDeeOverview.cpp

${OBJECTDIR}/quadtreenode.o: quadtreenode.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/quadtreenode.o quadtreenode.cpp

${OBJECTDIR}/lidar_loader.o: lidar_loader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/lidar_loader.o lidar_loader.cpp

${OBJECTDIR}/quadtree.o: quadtree.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/quadtree.o quadtree.cpp

${OBJECTDIR}/test.o: test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/test.o test.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Release
	${RM} dist/Release/GNU-Linux-x86/svncheckoutstuff

# Subprojects
.clean-subprojects:
