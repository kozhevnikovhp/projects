# Create a library called "serial-connection" which includes the source file.
# The extension is already found. Any number of sources could be listed here.

add_library (serial-connection
    modem-gct.cpp
    serial-connection.cpp
    )

# Make sure the compiler can find include files for our serial-connection library
# when other libraries or executables link to it
target_include_directories (serial-connection PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



