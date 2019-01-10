cmake_minimum_required(VERSION 2.8)

# Setup testing
enable_testing()
FIND_PACKAGE(GTest REQUIRED)
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIR})

# Add executable called "lte-manager-unit-tests" that is built from the source files
ADD_EXECUTABLE(lte-manager-unit-tests
    lte-manager-unit-tests.cpp
    ../json.cpp
    ../log.cpp
    ../misc.cpp
    ../modem-gtc.cpp
    )

#C++11 support
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#real or pseudo-modem (if real modem, remove this line)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPSEUDO_MODEM")

TARGET_LINK_LIBRARIES(lte-manager-unit-tests gtest gtest_main pthread)

ADD_TEST(lte-manager-unit-tests lte-manager-unit-tests)

