## \file udpgen/CMakeLists.txt
#

SET (SRCS
    udpgen.cpp
)


ADD_EXECUTABLE (udpgen
    ${SRCS}
)

TARGET_LINK_LIBRARIES (udpgen
                      network
                      portable
)

INSTALL (TARGETS udpgen
    RUNTIME DESTINATION ${INSTALL_BIN_DIR}
)

# EOF CMakeLists.txt
