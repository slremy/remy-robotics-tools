include_directories (${CMAKE_SOURCE_DIR}/src)

include_directories(${USB_INCLUDE_DIRS})

add_executable(testarm testarm.cc)
target_link_libraries(testarm owimotorcontroller)


IF(BUILD_matlab_INTERFACES)
SET(OWIARM_INTERPRETED_FILES
	${OWIARM_INTERPRETED_FILES}
	testarm.m
)
ENDIF(BUILD_matlab_INTERFACES)

IF(BUILD_octave_INTERFACES)
SET(OWIARM_INTERPRETED_FILES
	${OWIARM_INTERPRETED_FILES}
	testarm_o.m
)
ENDIF(BUILD_octave_INTERFACES)

IF(BUILD_python_INTERFACES)
SET(OWIARM_INTERPRETED_FILES
	${OWIARM_INTERPRETED_FILES}
	testarm.py
)
ENDIF(BUILD_python_INTERFACES)

IF ("${CMAKE_BINARY_DIR}" MATCHES "^${CMAKE_SOURCE_DIR}$")
    MESSAGE("In source build -- no need to copy files.")
ELSE ("${CMAKE_BINARY_DIR}" MATCHES "^${CMAKE_SOURCE_DIR}$")
    FOREACH(file ${OWIARM_INTERPRETED_FILES})
      SET(src "${CMAKE_SOURCE_DIR}/examples/${file}")
      SET(tgt "${CMAKE_BINARY_DIR}/examples/${file}")
      execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${src} ${tgt})
    ENDFOREACH(file)
ENDIF ("${CMAKE_BINARY_DIR}" MATCHES "^${CMAKE_SOURCE_DIR}$")
