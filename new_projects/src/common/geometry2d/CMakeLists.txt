## \file geometry2d/CMakeLists.txt
#

SET (GEOMETRY2D_SRCS
    geometry2d.cpp
)


ADD_LIBRARY (geometry2d
    ${GEOMETRY2D_SRCS}
)

TARGET_LINK_LIBRARIES (geometry2d
)


# EOF CMakeLists.txt
