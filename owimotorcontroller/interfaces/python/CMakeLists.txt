# This is a CMake example for Python

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs REQUIRED)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

MESSAGE( STATUS "${PYTHON_LIBRARIES}")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	FIND_LIBRARY(CARBON_LIBRARY Carbon)
	FIND_LIBRARY(IOKIT_LIBRARY IOKit)
	MARK_AS_ADVANCED (CARBON_LIBRARY IOKIT_LIBRARY)
	SET(EXTRA_LIBS ${CARBON_LIBRARY} ${IOKIT_LIBRARY})
	SET(EXTRA_SRC ${CMAKE_SOURCE_DIR}/src/usbwrap_iokit.cc)
ELSE (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	find_package(USB REQUIRED)
	include_directories(${USB_INCLUDE_DIRS})
	SET(EXTRA_LIBS ${USB_LIBRARIES})
	SET(EXTRA_SRC  ${CMAKE_SOURCE_DIR}/src/usbwrap_libusb.cc)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src )

SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(owiswig.i PROPERTIES CPLUSPLUS ON)
#SET_SOURCE_FILES_PROPERTIES(owiswig.i PROPERTIES SWIG_FLAGS "-includeall")
SWIG_ADD_MODULE(owiswig python owiswig.i ${CMAKE_SOURCE_DIR}/src/owimotorcontroller.cc ${EXTRA_SRC})
SWIG_LINK_LIBRARIES(owiswig ${PYTHON_LIBRARIES} ${EXTRA_LIBS})
