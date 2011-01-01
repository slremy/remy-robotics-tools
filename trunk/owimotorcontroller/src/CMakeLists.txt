include_directories (.)

find_package(USB REQUIRED)
include_directories(${USB_INCLUDE_DIRS})


#add_library (owimotorcontroller STATIC owimotorcontroller.cc)


 ADD_LIBRARY(owimotorcontroller SHARED owimotorcontroller.cc)
 ADD_LIBRARY(owimotorcontroller-static STATIC owimotorcontroller.cc)
 # The library target "owimotorcontroller" already has a default OUTPUT_NAME of "owimotorcontroller", so we don't need to change it.
 # The library target "owimotorcontroller-static" has a default OUTPUT_NAME of "owimotorcontroller-static", so change it.
 SET_TARGET_PROPERTIES(owimotorcontroller-static PROPERTIES OUTPUT_NAME "owimotorcontroller")
 # Now the library target "owimotorcontroller-static" will be named "owimotorcontroller.lib" with MS tools.
 # This conflicts with the "owimotorcontroller.lib" import library corresponding to "owimotorcontroller.dll",
 # so we add a "lib" prefix (which is default on other platforms anyway):
 SET_TARGET_PROPERTIES(owimotorcontroller-static PROPERTIES PREFIX "lib")

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
