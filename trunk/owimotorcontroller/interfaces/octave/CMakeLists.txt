# This is a CMake example for Python

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
FIND_PROGRAM(mkoctfile NAME "mkoctfile" PATHS)

FIND_PACKAGE(Octave)
IF(OCTAVE_FOUND)
    INCLUDE_DIRECTORIES(${OCTAVE_INCLUDE_PATH})

    find_package(USB REQUIRED)
    INCLUDE_DIRECTORIES(${USB_INCLUDE_DIRS})

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src )

    SET(CMAKE_SWIG_FLAGS "")

    SET(SWIG_CXX_EXTENSION "cc")
    SET_SOURCE_FILES_PROPERTIES(owiswig.i PROPERTIES CPLUSPLUS ON)
    #SET_SOURCE_FILES_PROPERTIES(owiswig.i PROPERTIES SWIG_FLAGS "-includeall")

    SWIG_ADD_MODULE(owiswig_o octave owiswig.i ${CMAKE_SOURCE_DIR}/src/owimotorcontroller.cc)
    SWIG_LINK_LIBRARIES(owiswig_o ${OCTAVE_LIBRARIES} ${USB_LIBRARIES})

    set(OCTAVE_OWISWIG_MEX ${CMAKE_CURRENT_BINARY_DIR}/owiswig.oct)
    add_custom_command(
        OUTPUT ${OCTAVE_OWISWIG_MEX}
        COMMAND ${mkoctfile}
        ARGS -I${CMAKE_SOURCE_DIR}/src ${MEX_CXXFLAGS} -o ${OCTAVE_OWISWIG_MEX} -L${CMAKE_CURRENT_BINARY_DIR} -lowiswig_o \"owiswigOCTAVE_wrap.cc\"
    )

    add_custom_target(owiswig_octfile ALL DEPENDS ${OCTAVE_OWISWIG_MEX})
else(OCTAVE_FOUND)
    message(STATUS "Octave installation not found")
ENDIF(OCTAVE_FOUND)
