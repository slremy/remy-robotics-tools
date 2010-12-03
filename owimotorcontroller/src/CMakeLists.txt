include_directories (.)

find_package(USB REQUIRED)
include_directories(${USB_INCLUDE_DIRS})


add_library (owimotorcontroller owimotorcontroller.cc)
target_link_libraries (owimotorcontroller  ${USB_LIBRARIES})
