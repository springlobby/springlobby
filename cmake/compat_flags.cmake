
include(TestCXXAcceptsFlag)

CHECK_CXX_ACCEPTS_FLAG("-std=c++11" ACCEPTSFLAGCXX11)
CHECK_CXX_ACCEPTS_FLAG("-std=c++17" ACCEPTSFLAGCXX17)

MACRO(AddSTDFlag FLAG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG} ")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${FLAG} ")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${FLAG} ")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${FLAG} ")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${FLAG} ")
ENDMACRO(AddSTDFlag FLAG)

if (ACCEPTSFLAGCXX17)
	AddSTDFlag("-std=c++17")
elseif(ACCEPTSFLAGCXX11)
	AddSTDFlag("-std=c++11")
else()
	message(FATAL_ERROR "you need a c++11 compatible compiler")
endif()

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
