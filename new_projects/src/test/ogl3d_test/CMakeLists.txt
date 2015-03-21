## \file ogl3d_test/CMakeLists.txt
#

FIND_PACKAGE (Qt4 REQUIRED)
QT_MINIMUM_REQUIRED (VERSION 4.5.2)
INCLUDE (${QT_USE_FILE})

include_directories(${QT_INCLUDES})

SET (SRCS
    main.cpp
    main_statusbar.cpp
    main_window.cpp
)

SET (MOC_HDRS
    main_statusbar.h
    main_window.h
)

QT4_GENERATE_MOC (main_statusbar.cpp main_statusbar.moc)
QT4_GENERATE_MOC (main_window.cpp main_window.moc)

QT4_WRAP_CPP (MOC_SRCS ${MOC_HDRS})


ADD_EXECUTABLE (ogl3d_test
               ${SRCS}
               ${MOC_SRCS}
)

TARGET_LINK_LIBRARIES (ogl3d_test
                      ogl
                      ${QT_LIBRARIES}
                      ${OPENGL_LIBRARY}
)

INSTALL (TARGETS ogl3d_test
    RUNTIME DESTINATION ${INSTALL_BIN_DIR}
)

# EOF CMakeLists.txt
