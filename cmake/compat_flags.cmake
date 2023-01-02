
include(TestCXXAcceptsFlag)

macro(AddSTDFlag FLAG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG} ")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${FLAG} ")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${FLAG} ")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${FLAG} ")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${FLAG} ")
endmacro()

if(NOT DEFINED LTO_FLAGS)
	set(LTO_FLAGS "")

	set(LTO FALSE CACHE BOOL "Link Time Optimizations (LTO)")
	if(LTO)
		check_cxx_accepts_flag("-flto" HAS_LTO_FLAG)
		if(HAS_LTO_FLAG)
			set(LTO_FLAGS "${LTO_FLAGS} -flto")
		else()
			set(LTO_FLAGS "${LTO_FLAGS} -flto")
		endif()
	endif()

	set(LTO_WHOPR FALSE CACHE BOOL "Link Time Optimizations (LTO) - Whole program optimizer (WHOPR)")
	if(LTO_WHOPR)
		check_cxx_accepts_flag("-fwhopr" HAS_LTO_WHOPR_FLAG)
		if(HAS_LTO_WHOPR_FLAG)
			set(LTO_FLAGS "${LTO_FLAGS} -fwhopr")
		endif()
	endif()

	if(LTO AND LTO_WHOPR)
		message(FATAL_ERROR "LTO and LTO_WHOPR are mutually exclusive, please enable only one at a time.")
	endif()
endif()

# Add Link-Time-Optimization flags, if supported (GCC >= 4.5) and enabled
set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS}           ${LTO_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    ${LTO_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LTO_FLAGS}")
