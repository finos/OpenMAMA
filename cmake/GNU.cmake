include(ExternalProject)

add_definitions(-D_GNU_SOURCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wmissing-prototypes -Wstrict-prototypes -pedantic-errors -fPIC")

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
	message(FATAL_ERROR "GCC versions prior to 4.8 unsupported (detected: ${CMAKE_CXX_COMPILER_VERSION})" )
endif()

# Proper regex support added in GCC 4.9+
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
	message(STATUS "GCC version ${CMAKE_CXX_COMPILER_VERSION} does not have regex support - reverting to libc's regex.h")
	include(CheckIncludeFile)
	CHECK_INCLUDE_FILE(regex.h REQUIRES_LIBC_REGEX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DREQUIRES_LIBC_REGEX=1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DREQUIRES_LIBC_REGEX=1")
endif()

macro(csharp_build_project target project depends)
	ExternalProject_Add(${target}
			SOURCE_DIR "."
			BUILD_COMMAND ""
			CONFIGURE_COMMAND dotnet build -c Release -o ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${project}
			INSTALL_COMMAND ""
			LOG_BUILD 1
            DEPENDS ${depends})
endmacro()