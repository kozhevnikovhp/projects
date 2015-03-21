## \file gransim2d_console/CMakeLists.txt
#

SET (SRCS
    main.cpp
)


ADD_EXECUTABLE (gransim2d_console
    ${SRCS}
)

TARGET_LINK_LIBRARIES (gransim2d_console
                      engine
)

INSTALL (TARGETS gransim2d_console
    RUNTIME DESTINATION ${INSTALL_BIN_DIR}
)

# EOF CMakeLists.txt
