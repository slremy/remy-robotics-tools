include_directories (${CMAKE_SOURCE_DIR}/src)

#FIND_PACKAGE(Matlab)
INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})

find_package(USB REQUIRED)
include_directories(${USB_INCLUDE_DIRS})

#add_executable(owiswig_m owiswig_m.cc)
#target_link_libraries(owiswig_m owimotorcontroller)
ADD_LIBRARY(owiswig_m MODULE owiswig_m.cc)
TARGET_LINK_LIBRARIES(owiswig_m ${MATLAB_LIBRARIES} ${USB_LIBRARIES} owimotorcontroller)
