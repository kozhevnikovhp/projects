## \file gransim2d_gui/CMakeLists.txt
#

FIND_PACKAGE (Qt4 REQUIRED)
QT_MINIMUM_REQUIRED (VERSION 4.5.2)
INCLUDE (${QT_USE_FILE})

FIND_PACKAGE(OpenGL REQUIRED)
IF(OPENGL_FOUND)
	MESSAGE(STATUS "OPENGL_LIBRARIES: " ${OPENGL_LIBRARIES})
ENDIF(OPENGL_FOUND)

include_directories(${QT_INCLUDES})

SET (SRCS
    gransim2d_renderer.cpp
    main.cpp
    main_statusbar.cpp
    main_window.cpp
    projectview.cpp
)

SET (MOC_HDRS
    main_statusbar.h
    main_window.h
    projectview.h
)

QT4_GENERATE_MOC (main_statusbar.cpp main_statusbar.moc)
QT4_GENERATE_MOC (main_window.cpp main_window.moc)
QT4_GENERATE_MOC (projectview.cpp projectview.moc)

QT4_WRAP_CPP (MOC_SRCS ${MOC_HDRS})


ADD_EXECUTABLE (gransim2d_gui
               ${SRCS}
               ${MOC_SRCS}
)

TARGET_LINK_LIBRARIES (gransim2d_gui
                      engine
                      ogl
                      ${QT_LIBRARIES}
                      ${OPENGL_LIBRARY}
)

INSTALL (TARGETS gransim2d_gui
    RUNTIME DESTINATION ${INSTALL_BIN_DIR}
)

# EOF CMakeLists.txt
