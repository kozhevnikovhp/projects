## \file engime/CMakeLists.txt
#

SET (SRCS
    design_text_string.cpp
    engine.cpp
    geometry2d_object.cpp
    global.cpp
    gransim2d_design.cpp
    gransim2d_object.cpp
    material.cpp
    materialized_object.cpp
    particle2d.cpp
    particle_shape_vertex.cpp
    particle_source.cpp
    particle_type.cpp
    simcell_2d.cpp
    text_const.cpp
)


ADD_LIBRARY (engine
    ${SRCS}
)

TARGET_LINK_LIBRARIES (engine
                      calc
                      geometry2d
                      portable
                      random
)


# EOF CMakeLists.txt
