# Add executable called "lte-manager" that is built from the source files
add_executable (lte-manager
    config.cpp
    json.cpp
    log.cpp
    lte-manager.cpp
    lte-parameters.cpp
    messaging-https.cpp
    misc.cpp
    modem-gtc.cpp
    serial-connection.cpp
    sniffer.cpp
    traffic-counter.cpp
    )

#C++11 support
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#real or pseudo-modem (if real modem, remove this line)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPSEUDO_MODEM")

target_link_libraries(lte-manager LINK_PUBLIC pcap rdkafka++ dbus-1 curl)

ADD_SUBDIRECTORY(unit_tests)

