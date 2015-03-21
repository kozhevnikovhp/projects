## \file random/CMakeLists.txt
#

SET (SRCS
    random.cpp
)


ADD_LIBRARY (random
    ${SRCS}
)

TARGET_LINK_LIBRARIES (random
)


# EOF CMakeLists.txt
