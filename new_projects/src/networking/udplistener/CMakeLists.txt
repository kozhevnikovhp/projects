## \file udplistener/CMakeLists.txt
#

SET (SRCS
    udplistener.cpp
)


ADD_EXECUTABLE (udplistener
    ${SRCS}
)

TARGET_LINK_LIBRARIES (udplistener
                      network
                      portable
)

INSTALL (TARGETS udplistener
    RUNTIME DESTINATION ${INSTALL_BIN_DIR}
)

# EOF CMakeLists.txt
