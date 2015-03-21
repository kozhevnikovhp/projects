## \file portable/CMakeLists.txt
#

SET (SRCS
    portable.cpp
    rgba.cpp
    text_string.cpp
    thread.cpp

)


ADD_LIBRARY (portable
    ${SRCS}
)

TARGET_LINK_LIBRARIES (portable
)


# EOF CMakeLists.txt
