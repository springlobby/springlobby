include(TestCXXAcceptsFlag)
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
				OUTPUT_VARIABLE GCC_VERSION)

include(CheckCXXCompilerFlag)

CHECK_CXX_ACCEPTS_FLAG("-std=c++11" ACCEPTSFLAGCXX11)
CHECK_CXX_ACCEPTS_FLAG("-std=c++17" ACCEPTSFLAGCXX17)

if (ACCEPTSFLAGCXX17)
	set (CMAKE_CXX_STANDARD 17)
elseif(ACCEPTSFLAGCXX11)
	set (CMAKE_CXX_STANDARD 11)
else()
	message(FATAL_ERROR "you need a c++11 compatible compiler")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)

If(NOT DEFINED LTO_FLAGS)
	Set(LTO_FLAGS "")

	Set(LTO       FALSE CACHE BOOL "Link Time Optimizations (LTO)")
	If    (LTO)
		CHECK_CXX_ACCEPTS_FLAG("-flto" HAS_LTO_FLAG)
		If    (HAS_LTO_FLAG)
			Set(LTO_FLAGS "${LTO_FLAGS} -flto")
		Else  (HAS_LTO_FLAG)
			Set(LTO_FLAGS "${LTO_FLAGS} -flto")
		EndIf (HAS_LTO_FLAG)
	EndIf (LTO)

	Set(LTO_WHOPR FALSE CACHE BOOL "Link Time Optimizations (LTO) - Whole program optimizer (WHOPR)")
	If    (LTO_WHOPR)
		CHECK_CXX_ACCEPTS_FLAG("-fwhopr" HAS_LTO_WHOPR_FLAG)
		If    (HAS_LTO_WHOPR_FLAG)
			Set(LTO_FLAGS "${LTO_FLAGS} -fwhopr")
		EndIf (HAS_LTO_WHOPR_FLAG)
	EndIf (LTO_WHOPR)

	If (LTO AND LTO_WHOPR)
		Message(FATAL_ERROR "LTO and LTO_WHOPR are mutually exclusive, please enable only one at a time." )
	EndIf (LTO AND LTO_WHOPR)

EndIf (NOT DEFINED LTO_FLAGS)

# Add Link-Time-Optimization flags, if supported (GCC >= 4.5) and enabled
Set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS}           ${LTO_FLAGS}")
Set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    ${LTO_FLAGS}")
Set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LTO_FLAGS}")
