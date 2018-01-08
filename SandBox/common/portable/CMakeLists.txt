## \file portable/CMakeLists.txt
#
PROJECT(portable)

IF (WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOS_WINDOWS")
ENDIF(WIN32)

IF (UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOS_UNIX")
ENDIF(UNIX)
SET (SRCS
    portable.cpp
    rgba.cpp
    text_string.cpp
    thread.cpp
)


ADD_LIBRARY (portable
    ${SRCS}
)

TARGET_LINK_LIBRARIES (portable
)

INSTALL(TARGETS portable
    DESTINATION lib)

# EOF CMakeLists.txt
