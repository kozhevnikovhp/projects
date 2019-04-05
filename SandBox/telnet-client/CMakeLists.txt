CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(telnet-client)

add_executable (telnet-client
    misc.cpp
    tcp.cpp
    tcp-client.cpp
    tcp-server.cpp
    telnet.cpp
    telnet-client.cpp
    telnet-gtc.cpp
)

#C++11 support
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


target_link_libraries(telnet-client LINK_PUBLIC )


