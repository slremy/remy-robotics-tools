# This is a CMake example for Python

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

find_package(USB REQUIRED)
INCLUDE_DIRECTORIES(${USB_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src )

SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(owiswig.i PROPERTIES CPLUSPLUS ON)
#SET_SOURCE_FILES_PROPERTIES(owiswig.i PROPERTIES SWIG_FLAGS "-includeall")
SWIG_ADD_MODULE(owiswig python owiswig.i ${CMAKE_SOURCE_DIR}/src/owimotorcontroller.cc)
SWIG_LINK_LIBRARIES(owiswig ${PYTHON_LIBRARIES} ${USB_LIBRARIES})

SET(OWIARM_PYTHON_FILES
	owiswig.py
)

IF ("${CMAKE_BINARY_DIR}" MATCHES "^${CMAKE_SOURCE_DIR}$")
    MESSAGE("In source build -- no need to copy Python files.")
ELSE ("${CMAKE_BINARY_DIR}" MATCHES "^${CMAKE_SOURCE_DIR}$")
    FOREACH(file ${OWIARM_PYTHON_FILES})
      SET(src "${CMAKE_SOURCE_DIR}/interfaces/python/${file}")
      SET(tgt "${CMAKE_BINARY_DIR}/interfaces/python/${file}")
      ADD_CUSTOM_COMMAND(DEPENDS ${src}
                         COMMAND ${CMAKE_COMMAND}
                         ARGS -E copy ${src} ${tgt}
                         OUTPUT ${tgt}
                         COMMENT "source copy")
    ENDFOREACH(file)
ENDIF ("${CMAKE_BINARY_DIR}" MATCHES "^${CMAKE_SOURCE_DIR}$")
