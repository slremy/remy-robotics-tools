PROJECT(libowimotorcontroller)

cmake_minimum_required(VERSION 2.6)

OPTION(BUILD_EXAMPLES "Build example programs" ON)
OPTION(BUILD_matlab_INTERFACES "Build (fake) matlab SWIG interfaces" OFF)
OPTION(BUILD_octave_INTERFACES "Build octave SWIG interfaces" OFF)
OPTION(BUILD_java_INTERFACES "Build java SWIG interfaces" OFF)
OPTION(BUILD_python_INTERFACES "Build python SWIG interfaces" OFF)

SET(BUILD_matlab_INTERFACES ${BUILD_matlab_INTERFACES})
SET(BUILD_octave_INTERFACES ${BUILD_octave_INTERFACES})
SET(BUILD_java_INTERFACES ${BUILD_java_INTERFACES})
SET(BUILD_python_INTERFACES ${BUILD_python_INTERFACES})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Modules/")

include (SetupDirectories)

set(CMAKE_C_FLAGS "-Wall -O3 -g")

add_subdirectory (src)
# Add examples
IF(BUILD_EXAMPLES)
    add_subdirectory (examples)
ENDIF()

add_subdirectory (interfaces)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

