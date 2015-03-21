# Main CMakeLists.txt
# Setup global options and include all other sub-projects.

project(EVERYTHING)

MESSAGE(STATUS "CMAKE_SYSTEM = " ${CMAKE_SYSTEM})
MESSAGE(STATUS "CMAKE_SYSTEM_NAME = " ${CMAKE_SYSTEM_NAME})
MESSAGE(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR = " ${CMAKE_HOST_SYSTEM_PROCESSOR})

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/maint/cmake")

INCLUDE(${CMAKE_MODULE_PATH}/build_types.cmake)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

set(CMAKE_BUILD_TYPE Debug)
IF (NOT ((${CMAKE_BUILD_TYPE} STREQUAL "Debug") OR
         (${CMAKE_BUILD_TYPE} STREQUAL "Release") OR
         (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo") OR
         (${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel") OR
         (${CMAKE_BUILD_TYPE} STREQUAL "Release_O3") OR
         (${CMAKE_BUILD_TYPE} STREQUAL "Release_O2") OR
         (${CMAKE_BUILD_TYPE} STREQUAL "Release_O1") OR
         (${CMAKE_BUILD_TYPE} STREQUAL "Release_O0") OR
         (${CMAKE_BUILD_TYPE} STREQUAL "Release_WithAsserts") OR
         (${CMAKE_BUILD_TYPE} STREQUAL "ProfilePerformance")))
    MESSAGE (FATAL_ERROR "Unknown build type '${CMAKE_BUILD_TYPE}'")
ENDIF ()

IF(LINUX64)
    MESSAGE("Building 64-bit ${CMAKE_BUILD_TYPE} version")
ELSE()
    MESSAGE("Building 32-bit ${CMAKE_BUILD_TYPE} version")
ENDIF(LINUX64)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# System specific definitions

IF (WIN32)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOS_WINDOWS")
ENDIF(WIN32)

IF (UNIX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOS_UNIX")
ENDIF(UNIX)

IF(${CMAKE_SYSTEM_NAME} MATCHES SunOS|Solaris)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOS_SOLARIS")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES SunOS|Solaris)


include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckTypeSize)
include(CheckCXXSourceCompiles)


include_directories(
    ${CMAKE_SOURCE_DIR}/src/ 
    ${CMAKE_SOURCE_DIR}/src/common/ 
)

link_directories(
)

###
# Installation of the product

set (CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})
set (INSTALL_BIN_DIR bin)
set (INSTALL_LIB_DIR lib)


###
# Sources
ADD_SUBDIRECTORY(src)

# EOF CMakeLists.txt
