# Add executable called "lte-manager" that is built from the source files
add_executable (lte-manager
    config.cpp
    const.cpp
    curl-lib.cpp
    fw-upgrade.cpp
    json.cpp
    log.cpp
    lte-manager.cpp
    lte-parameters.cpp
    misc.cpp
    modem-gtc.cpp
    myxid.cpp
    serial-connection.cpp
    sniffer.cpp
    sniffer-pcap.cpp
    sniffer-raw-sockets.cpp
    traffic-counter.cpp
    verbosity.cpp
    version.cpp
    )

#C++11 support
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#real or pseudo-modem (if real modem, remove this line)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPSEUDO_MODEM")

#how to listen to traffic? Use PCAP or raw sockets. If raw sockets, remove next line
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_PCAP")


target_link_libraries(lte-manager LINK_PUBLIC pcap rdkafka++ dbus-1 curl)

ADD_SUBDIRECTORY(unit_tests)

