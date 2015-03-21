# Magnus Homann svn hack, stolen from http://svn.osgeo.org/qgis/trunk/qgis/cmake_templates/svnscript.cmake.in_cmake

SET(CMAKE_BACKWARDS_COMPATIBILITY "2.4")

# SVN version
FIND_PROGRAM(SVNVERSION svnversion)

IF(SVNVERSION)
    EXEC_PROGRAM(${SVNVERSION} ARGS "@CMAKE_CURRENT_SOURCE_DIR@" OUTPUT_VARIABLE SVN_VERSION_TMP)
ELSE(SVNVERSION)
    SET(SVN_VERSION_TMP unknown)
ENDIF(SVNVERSION)

FIND_PROGRAM(UNAME uname)

# OS and arch
IF(UNAME)
    EXEC_PROGRAM(${UNAME} ARGS "-o" OUTPUT_VARIABLE OS_NAME_TMP)
    EXEC_PROGRAM(${UNAME} ARGS "-m" OUTPUT_VARIABLE ARCH_NAME_TMP)
ELSE(UNAME)
    SET(OS_NAME_TMP unknown)
    SET(ARCH_NAME_TMP unknown)
ENDIF(UNAME)

# Compiler version
EXEC_PROGRAM(g++ ARGS "--version" OUTPUT_VARIABLE _gcc_version_info)

STRING(REGEX MATCH " [34]\\.[0-9]\\.[0-9]" GCC_VERSION_TMP "${_gcc_version_info}")

# gcc on mac just reports: "gcc (GCC) 3.3 20030304 ..." without the patch level, handle this here:
IF(NOT GCC_VERSION_TMP)
    STRING(REGEX REPLACE ".*\\(GCC\\).* ([34]\\.[0-9]) .*" "\\1.0" GCC_VERSION_TMP "${_gcc_version_info}")
ENDIF(NOT GCC_VERSION_TMP)

# Build date
FIND_PROGRAM(DATE date)
IF (DATE)
    EXEC_PROGRAM(${DATE} ARGS "+\"%d %b %Y\"" OUTPUT_VARIABLE BUILD_DATE_TMP)
ELSE (DATE)
    SET (BUILD_DATE_TMP unknown)
ENDIF (DATE)

# Build type (debug/release)
STRING(STRIP ${SVN_VERSION_TMP} SVN_VERSION)
STRING(STRIP ${OS_NAME_TMP}     OS_NAME)
STRING(STRIP ${ARCH_NAME_TMP}   ARCH_NAME)
STRING(STRIP ${GCC_VERSION_TMP} GCC_VERSION)
STRING(STRIP ${BUILD_DATE_TMP}  BUILD_DATE)
STRING(STRIP @CMAKE_BUILD_TYPE@  BUILD_TYPE_TMP)
STRING(TOLOWER ${BUILD_TYPE_TMP} BUILD_TYPE)

####################################################################
# Configure version.h
CONFIGURE_FILE("@CMAKE_CURRENT_SOURCE_DIR@/version.h.in"
               "@CMAKE_CURRENT_BINARY_DIR@/version.h")

# EOF version.cmake
