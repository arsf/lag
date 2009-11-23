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
OBJECTDIR=build/Debug/GNU-Linux-x86

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/LASloader.o \
	${OBJECTDIR}/quadtreestructs.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/quadtreenode.o \
	${OBJECTDIR}/lidar_loader.o \
	${OBJECTDIR}/quadtree.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-I /users/rsg/chrfi/liblas_x64/include -pg -pedantic
CXXFLAGS=-I /users/rsg/chrfi/liblas_x64/include -pg -pedantic

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../liblas_x64/lib -llas

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} dist/Debug/GNU-Linux-x86/lidar_point_practise_1.1

dist/Debug/GNU-Linux-x86/lidar_point_practise_1.1: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o dist/Debug/GNU-Linux-x86/lidar_point_practise_1.1 ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/LASloader.o: LASloader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -Wall -o ${OBJECTDIR}/LASloader.o LASloader.cpp

${OBJECTDIR}/quadtreestructs.o: quadtreestructs.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -Wall -o ${OBJECTDIR}/quadtreestructs.o quadtreestructs.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -Wall -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/quadtreenode.o: quadtreenode.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -Wall -o ${OBJECTDIR}/quadtreenode.o quadtreenode.cpp

${OBJECTDIR}/lidar_loader.o: lidar_loader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -Wall -o ${OBJECTDIR}/lidar_loader.o lidar_loader.cpp

${OBJECTDIR}/quadtree.o: quadtree.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -Wall -o ${OBJECTDIR}/quadtree.o quadtree.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/lidar_point_practise_1.1

# Subprojects
.clean-subprojects:
