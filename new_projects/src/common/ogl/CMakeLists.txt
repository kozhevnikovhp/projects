## \file ogl/CMakeLists.txt
#

INCLUDE (${CMAKE_MODULE_PATH}/qtutils.cmake)

FIND_PACKAGE (Qt4 REQUIRED)
QT_MINIMUM_REQUIRED (VERSION 4.5.2)
SET(QT_USE_QTOPENGL TRUE)
INCLUDE (${QT_USE_FILE})

FIND_PACKAGE(OpenGL REQUIRED)
IF(OPENGL_FOUND)
	MESSAGE(STATUS "OPENGL_LIBRARY = " ${OPENGL_LIBRARY})
ENDIF(OPENGL_FOUND)

include_directories(${QT_INCLUDES})

SET (SRCS
    opengl_widget.cpp
    opengl_2d_widget.cpp
    opengl_2d_chart.cpp
    opengl_2d_editor.cpp
    opengl_3d_widget.cpp
)

SET (MOC_HDRS
    opengl_widget.h
    opengl_2d_widget.h
    opengl_2d_chart.h
    opengl_2d_editor.h
    opengl_3d_widget.h
)

QT4_GENERATE_MOC (opengl_widget.cpp     opengl_widget.moc)
QT4_GENERATE_MOC (opengl_2d_widget.cpp  opengl_2d_widget.moc)
QT4_GENERATE_MOC (opengl_2d_chart.cpp  opengl_2d_chart.moc)
QT4_GENERATE_MOC (opengl_2d_editor.cpp  opengl_2d_editor.moc)
QT4_GENERATE_MOC (opengl_3d_widget.cpp  opengl_3d_widget.moc)

QT4_WRAP_CPP (MOC_SRCS ${MOC_HDRS})

ADD_LIBRARY (ogl
    ${SRCS}
    ${MOC_SRCS}
)

TARGET_LINK_LIBRARIES (ogl
                      ${QT_LIBRARIES}
                      ${OPENGL_LIBRARY}
)


# EOF CMakeLists.txt
