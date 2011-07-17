include_directories (.)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	FIND_LIBRARY(CARBON_LIBRARY Carbon)
	FIND_LIBRARY(IOKIT_LIBRARY IOKit)
	MARK_AS_ADVANCED (CARBON_LIBRARY IOKIT_LIBRARY)
	SET(EXTRA_LIBS ${CARBON_LIBRARY} ${IOKIT_LIBRARY})
	SET(EXTRA_SRC usbwrap_iokit.cc)
ELSE (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

	find_package(USB REQUIRED)
	include_directories(${USB_INCLUDE_DIRS})
	SET(EXTRA_LIBS ${USB_LIBRARIES})
	SET(EXTRA_SRC  usbwrap_libusb.cc)
	SET(EXTRA_SRC  usbwrap_libusb.cc)

ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")



ADD_LIBRARY(owimotorcontroller SHARED owimotorcontroller.cc ${EXTRA_SRC})
ADD_LIBRARY(owimotorcontroller-static STATIC owimotorcontroller.cc ${EXTRA_SRC})
# The library target "owimotorcontroller" already has a default OUTPUT_NAME of "owimotorcontroller", so we don't need to change it.
# The library target "owimotorcontroller-static" has a default OUTPUT_NAME of "owimotorcontroller-static", so change it.
SET_TARGET_PROPERTIES(owimotorcontroller-static PROPERTIES OUTPUT_NAME "owimotorcontroller")
# Now the library target "owimotorcontroller-static" will be named "owimotorcontroller.lib" with MS tools.
# This conflicts with the "owimotorcontroller.lib" import library corresponding to "owimotorcontroller.dll",
# so we add a "lib" prefix (which is default on other platforms anyway):
SET_TARGET_PROPERTIES(owimotorcontroller-static PROPERTIES PREFIX "lib")


target_link_libraries (owimotorcontroller ${EXTRA_LIBS})

install (TARGETS owimotorcontroller
DESTINATION "${PROJECT_LIBRARY_INSTALL_DIR}")

# Install the header file
install (FILES "owimotorcontroller.h owimotorcontroller.hpp"
DESTINATION ${PROJECT_INCLUDE_INSTALL_DIR})

IF(UNIX OR APPLE)
# Produce a pkg-config file for linking against the shared lib
configure_file ("libowimotorcontroller.pc.in" "libowimotorcontroller.pc" @ONLY)
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/libowimotorcontroller.pc"
DESTINATION "${PROJECT_LIBRARY_INSTALL_DIR}/pkgconfig")
ENDIF()
