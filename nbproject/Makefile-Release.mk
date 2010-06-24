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
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include lag-Makefile.mk

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/main.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/quadtreenode.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/quadtreestructs.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/LASloader.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/LASsaver.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/cacheminder.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/pointbucket.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/collisiondetection.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/ASCIIloader.o \
	${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/quadtree.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/GNU-Linux-x86/lag

dist/Release/GNU-Linux-x86/lag: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lag ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/main.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/main.o /users/rsg/chrfi/git_repos/lag/main.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/quadtreenode.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/quadtreenode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/quadtreenode.o /users/rsg/chrfi/git_repos/lag/quadtreenode.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/quadtreestructs.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/quadtreestructs.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/quadtreestructs.o /users/rsg/chrfi/git_repos/lag/quadtreestructs.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/LASloader.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/LASloader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/LASloader.o /users/rsg/chrfi/git_repos/lag/LASloader.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/LASsaver.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/LASsaver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/LASsaver.o /users/rsg/chrfi/git_repos/lag/LASsaver.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/cacheminder.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/cacheminder.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/cacheminder.o /users/rsg/chrfi/git_repos/lag/cacheminder.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/pointbucket.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/pointbucket.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/pointbucket.o /users/rsg/chrfi/git_repos/lag/pointbucket.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/collisiondetection.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/collisiondetection.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/collisiondetection.o /users/rsg/chrfi/git_repos/lag/collisiondetection.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/ASCIIloader.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/ASCIIloader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/ASCIIloader.o /users/rsg/chrfi/git_repos/lag/ASCIIloader.cpp

${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/quadtree.o: nbproject/Makefile-${CND_CONF}.mk /users/rsg/chrfi/git_repos/lag/quadtree.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/users/rsg/chrfi/git_repos/lag/quadtree.o /users/rsg/chrfi/git_repos/lag/quadtree.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Release
	${RM} dist/Release/GNU-Linux-x86/lag

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
