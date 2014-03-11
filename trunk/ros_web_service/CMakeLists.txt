cmake_minimum_required(VERSION 2.8.3)
project(ros_web_service)

find_package(catkin REQUIRED COMPONENTS 
#  gazebo_ros
)

include_directories(include ${catkin_INCLUDE_DIRS} ${GAZEBO_INCLUDE_DIRS} ${SDFormat_INCLUDE_DIRS})

# Build whatever you need here
#add_library(...) # TODO

catkin_package(
    DEPENDS 
    CATKIN_DEPENDS
    INCLUDE_DIRS 
    LIBRARIES 
)