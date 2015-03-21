# qtutils.cmake

# Check Qt version.
# Usage: QT_MINIMUM_REQUIRED (VERSION <major>[.<minor>[.<patch>]])
# Note: minor and patch parts are optional.
# Example: QT_MINIMUM_REQUIRED (VERSION 4.5.0)
#
MACRO (QT_MINIMUM_REQUIRED VERSION VERSION_REQUIRED)

    #MESSAGE ("VERSION_REQUIRED = ${VERSION_REQUIRED}")
    #MESSAGE ("QTVERSION = ${QTVERSION}")

    IF (NOT ${VERSION} STREQUAL "VERSION")
        MESSAGE (SEND_ERROR "Unknown keyword ${VERSION}.  Should be VERSION")
        RETURN()
    ENDIF()

    IF (${QTVERSION} VERSION_LESS ${VERSION_REQUIRED})
        MESSAGE (SEND_ERROR "Qt ${VERSION_REQUIRED} or higher is required.  You are running version ${QTVERSION}")
        RETURN()
    ENDIF()

ENDMACRO()

# EOF qtutils.cmake
