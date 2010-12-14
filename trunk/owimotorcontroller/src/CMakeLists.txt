include_directories (.)

find_package(USB REQUIRED)
include_directories(${USB_INCLUDE_DIRS})


add_library (owimotorcontroller owimotorcontroller.cc)
target_link_libraries (owimotorcontroller  ${USB_LIBRARIES})


install (TARGETS owimotorcontroller
  DESTINATION "${PROJECT_LIBRARY_INSTALL_DIR}")

# Install the header file
install (FILES "owimotorcontroller.h"
  DESTINATION ${PROJECT_INCLUDE_INSTALL_DIR})

IF(UNIX OR APPLE)
  # Produce a pkg-config file for linking against the shared lib
  configure_file ("libowimotorcontroller.pc.in" "libowimotorcontroller.pc" @ONLY)
  install (FILES "${CMAKE_CURRENT_BINARY_DIR}/libowimotorcontroller.pc"
    DESTINATION "${PROJECT_LIBRARY_INSTALL_DIR}/pkgconfig")
ENDIF()
