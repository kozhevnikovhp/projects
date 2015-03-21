## \file network/CMakeLists.txt
#

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSOCKETS_BSD")
IF(${CMAKE_SYSTEM_NAME} MATCHES SunOS|Solaris)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOS_SOLARIS")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES SunOS|Solaris)


SET (NETWORK_SRCS
    hardware.cpp
    icmp.cpp
    ip.cpp
    raw.cpp
    tcp.cpp
    misc.cpp
    udp.cpp
)


ADD_LIBRARY (network
    ${NETWORK_SRCS}
)

#socket libs
set (SOCKET_LIBRARIES)
IF(${CMAKE_SYSTEM_NAME} MATCHES SunOS|Solaris)
   set(SOCKET_LIBRARIES ${SOCKET_LIBRARIES} socket nsl)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES SunOS|Solaris)

TARGET_LINK_LIBRARIES (network
                      ${SOCKET_LIBRARIES}
)


# EOF CMakeLists.txt
