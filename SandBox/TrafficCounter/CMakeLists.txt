cmake_minimum_required(VERSION 2.8)

project(TrafficCounter)

#INCLUDE_DIRECTORIES("../common")

SET (SRCS
    main.cpp
    misc.cpp
    portable.cpp
    sniffer.cpp
)
IF (WIN32)
SET (SRCS
    ${SRCS}
    IpHelper.cpp
)
ENDIF (WIN32)

IF (WIN32)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOS_WINDOWS -DSOCKETS_WSA")
ELSEIF (UNIX)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOS_UNIX -DSOCKETS_BSD")
ENDIF(UNIX)

#C++11 support
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})

IF (WIN32)
  TARGET_LINK_LIBRARIES(TrafficCounter wsock32 ws2_32 iphlpapi)
ENDIF(WIN32)

#SET(CMAKE_EXE_LINKER_FLAGS " -static")
