cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
#set(ROS_BUILD_TYPE RelWithDebInfo)

rosbuild_init()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	FIND_LIBRARY(CARBON_LIBRARY Carbon)
	FIND_LIBRARY(IOKIT_LIBRARY IOKit)
	MARK_AS_ADVANCED (CARBON_LIBRARY IOKIT_LIBRARY)
	SET(EXTRA_LIBS ${CARBON_LIBRARY} ${IOKIT_LIBRARY})
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#common commands for building c++ executables and libraries
#rosbuild_add_library(${PROJECT_NAME} src/example.cpp)
rosbuild_add_executable(joy_node src/joy_node.cpp)

target_link_libraries(joy_node  ${EXTRA_LIBS})