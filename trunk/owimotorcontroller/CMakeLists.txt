PROJECT(libowimotorcontroller)

cmake_minimum_required(VERSION 2.6)

OPTION(BUILD_EXAMPLES "Build example programs" ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Modules/")

include (SetupDirectories)

set(CMAKE_C_FLAGS "-Wall -O3 -g")

add_subdirectory (src)
# Add examples
IF(BUILD_EXAMPLES)
  add_subdirectory (examples)
ENDIF()



# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

