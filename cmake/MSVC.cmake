include(ExternalProject)

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
	set (OPENMAMA_LIBRARY_SUFFIX "mdd")
	set (OPENMAMA_MAMAJNI_TARGET_SUFFIX "d")
	set (OPENMAMA_LIBRARY_STATIC_SUFFIX "mtd")
else ()
	set (OPENMAMA_LIBRARY_SUFFIX "md")
	set (OPENMAMA_LIBRARY_STATIC_SUFFIX "mt")
endif ()

add_definitions(
	-DXML_STATIC 
	-DFD_SETSIZE=1024 
	-DNOWINMESSAGES 
	-DHAVE_WOMBAT_MSG 
	-DREFRESH_TRANSPORT 
	-D_CRT_SECURE_NO_WARNINGS 
	-D_CRT_NONSTDC_NO_WARNING
	-D_WINSOCK_DEPRECATED_NO_WARNINGS
	-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
	-DOPENMAMA_LIBRARY_SUFFIX=${OPENMAMA_LIBRARY_SUFFIX}
)

# Force to always compile with W4
string(REGEX REPLACE "/W3" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Ensure PDB files are always created - even for release builds
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

set (CMAKE_SHARED_LIBRARY_PREFIX "lib")
# On windows this file has historically been named libmamac for some reason. Will conform in future.
set (CMAKE_STATIC_LIBRARY_PREFIX "lib")
set (CMAKE_IMPORT_LIBRARY_PREFIX "lib")
set (CMAKE_SHARED_MODULE_PREFIX "lib")
set (OPENMAMA_MAMA_TARGET_SUFFIX "c")

if(CMAKE_CL_64)
	set(DEFAULT_INSTALL_PREFIX $ENV{ProgramW6432})
else()
	set(PROGRAMFILES "PROGRAMFILES(X86)") 
	set(DEFAULT_INSTALL_PREFIX $ENV{${PROGRAMFILES}})
endif()

set (DEFAULT_LIBEVENT_ROOT "${DEFAULT_INSTALL_PREFIX}/libevent")
set (DEFAULT_PROTON_ROOT   "${DEFAULT_INSTALL_PREFIX}/Proton")
set (DEFAULT_APR_ROOT      "${DEFAULT_INSTALL_PREFIX}/APR")
set (DEFAULT_GTEST_ROOT    "${DEFAULT_INSTALL_PREFIX}/googletest-distribution")

include(ExternalProject)

macro(csharp_build_project target project output depends)
	ExternalProject_Add(${target}
			SOURCE_DIR "."
			BUILD_COMMAND ""
			CONFIGURE_COMMAND ${CMAKE_MAKE_PROGRAM} /p:PreBuildEvent= /p:PostBuildEvent= /p:OutputPath=${CMAKE_CURRENT_BINARY_DIR} /p:Configuration=Release /p:BuildPath=${CMAKE_CURRENT_BINARY_DIR}/build ${msbuild_project_assembly_flag} ${CMAKE_CURRENT_SOURCE_DIR}/${project}
			INSTALL_COMMAND ""
			LOG_BUILD 1
            DEPENDS ${depends})
endmacro()
