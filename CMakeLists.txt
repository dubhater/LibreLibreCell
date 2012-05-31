cmake_minimum_required(VERSION 2.6)

project(LibreLibreCell)

execute_process(COMMAND ${PROJECT_SOURCE_DIR}/version.sh WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE LIBRELIBRECELL_VERSION_GITREV OUTPUT_STRIP_TRAILING_WHITESPACE)
add_definitions("-DLIBRELIBRECELL_VERSION=\"${LIBRELIBRECELL_VERSION_GITREV}\"")

add_definitions("-Wall -pedantic")

add_subdirectory(src)

install(FILES README.txt DESTINATION share/doc/librelibrecell/)
