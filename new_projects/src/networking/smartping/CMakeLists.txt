## \file smartping/CMakeLists.txt
#

SET (SRCS
    smartping.cpp
)


ADD_EXECUTABLE (smartping
    ${SRCS}
)

TARGET_LINK_LIBRARIES (smartping
                      network
                      portable
)

INSTALL (TARGETS smartping
    RUNTIME DESTINATION ${INSTALL_BIN_DIR}
)

# EOF CMakeLists.txt
