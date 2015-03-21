## \file src/CMakeLists.txt
#

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}) # for auto-generated headers

ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(networking)
ADD_SUBDIRECTORY(granular)

# EOF CMakeLists.txt
