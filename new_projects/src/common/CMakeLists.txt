## \file common/CMakeLists.txt
#

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}) # for auto-generated headers

ADD_SUBDIRECTORY(calc)
ADD_SUBDIRECTORY(geometry2d)
ADD_SUBDIRECTORY(network)
ADD_SUBDIRECTORY(ogl)
ADD_SUBDIRECTORY(portable)
ADD_SUBDIRECTORY(random)


# EOF CMakeLists.txt
