include_directories (${CMAKE_SOURCE_DIR}/src)

FIND_PACKAGE(Matlab-rwp)
INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})
include_directories(${USB_INCLUDE_DIRS})
IF(MATLAB_INCLUDE_DIR)
	if( DARWIN OR APPLE )
		set(MEXEXT "mexmac")
	elseif( UNIX )
		if( IS_X86_64 )
			set(MEXEXT "mexa64")
		else()
			set(MEXEXT "mexglx")
		endif()
	elseif( WIN64 )
		set(MEXEXT "mexw64")
	elseif( WIN32 OR CYGWIN OR WINDOWS )
		set(MEXEXT "mexw32")
	else()
		set(MEXEXT "mex")
	endif()

      SET(src "${CMAKE_SOURCE_DIR}/interfaces/matlab/usbowiarm.m")
      SET(tgt "${CMAKE_BINARY_DIR}/interfaces/matlab/usbowiarm.m")
      execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${src} ${tgt})

	ADD_LIBRARY(owiswig_m MODULE owiswig_m.cc)
	TARGET_LINK_LIBRARIES(owiswig_m ${MATLAB_LIBRARIES} ${USB_LIBRARIES} owimotorcontroller)
	set_target_properties(owiswig_m PROPERTIES
		PREFIX ""
		SUFFIX  ".${MEXEXT}"
	)
ELSE(MATLAB_INCLUDE_DIR)
    message(STATUS "MATLAB installation not found")
ENDIF(MATLAB_INCLUDE_DIR)

