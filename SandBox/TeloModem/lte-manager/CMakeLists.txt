# Add executable called "lte-manager" that is built from the source files
add_executable (lte-manager
    lte-manager.cpp
    lte-parameters.cpp
    misc.cpp
    sniffer.cpp
    traffic-counter.cpp
    )

#C++11 support
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Link the executable to the serial-connection library. Since the serial-connection library has
# public include directories we will use those link directories when building
# lte-status
target_link_libraries(lte-manager LINK_PUBLIC serial-connection pcap rdkafka++)


