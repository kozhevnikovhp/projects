## \file calc/CMakeLists.txt
#

PROJECT(calc)

include_directories("..")

SET (CALC_SRCS
    calc.cpp
)


ADD_LIBRARY (calc
    ${CALC_SRCS}
)

TARGET_LINK_LIBRARIES (calc
)


# EOF CMakeLists.txt
