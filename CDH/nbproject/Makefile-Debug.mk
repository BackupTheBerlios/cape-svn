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
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=
CCC=
CXX=
FC=
AS=

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/command_handling.o \
	${OBJECTDIR}/low_speed_link.o \
	${OBJECTDIR}/logger.o \
	${OBJECTDIR}/data_handling.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/config.o \
	${OBJECTDIR}/cd_queues.o


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
LDLIBSOPTIONS=-lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/cdh

dist/Debug/GNU-Linux-x86/cdh: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cdh ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/command_handling.o: command_handling.cpp 
	${MKDIR} -p ${OBJECTDIR}
	

${OBJECTDIR}/low_speed_link.o: low_speed_link.cpp 
	${MKDIR} -p ${OBJECTDIR}
	

${OBJECTDIR}/logger.o: logger.cpp 
	${MKDIR} -p ${OBJECTDIR}
	

${OBJECTDIR}/data_handling.o: data_handling.cpp 
	${MKDIR} -p ${OBJECTDIR}
	

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	

${OBJECTDIR}/config.o: config.cpp 
	${MKDIR} -p ${OBJECTDIR}
	

${OBJECTDIR}/cd_queues.o: cd_queues.cpp 
	${MKDIR} -p ${OBJECTDIR}
	

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/cdh

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
