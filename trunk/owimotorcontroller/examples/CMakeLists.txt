include_directories (${CMAKE_SOURCE_DIR}/src)

include_directories(${USB_INCLUDE_DIRS})

add_executable(testarm testarm.cc)
target_link_libraries(testarm owimotorcontroller)
