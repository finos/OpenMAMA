include(ExternalProject)

add_definitions(-D_GNU_SOURCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wmissing-prototypes -Wstrict-prototypes -pedantic-errors -fPIC")

macro(csharp_build_project target project output depends)
	ExternalProject_Add(${target}
			SOURCE_DIR "."
			BUILD_COMMAND ""
			CONFIGURE_COMMAND dotnet build -p:DefineConstants=_GNU -c Release -o ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${project}
			INSTALL_COMMAND ""
			LOG_BUILD 1
            DEPENDS ${depends})
endmacro()