cmake_minimum_required (VERSION 2.8.3)

function (get_component_version COMPONENT)
	file(STRINGS ${CMAKE_SOURCE_DIR}/VERSION VERSION)

 	 string(REPLACE "." ";" VERSION_LIST ${VERSION})

	 list(GET VERSION_LIST 0 MAJOR)
	 list(GET VERSION_LIST 1 MINOR)
	 list(GET VERSION_LIST 2 RELEASE)

	 set(${COMPONENT}_VERSION_MAJOR ${MAJOR} PARENT_SCOPE)
	 set(${COMPONENT}_VERSION_MINOR ${MINOR} PARENT_SCOPE)
	 set(${COMPONENT}_VERSION_RELEASE ${RELEASE} PARENT_SCOPE)
endfunction(get_component_version)

macro(mamac_binary name)
	add_executable(${name} ${name}.c)
	target_link_libraries(${name} mama ${ARGN})
	install(TARGETS ${name}
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
	if (WIN32 AND MSVC)
		install(FILES $<TARGET_PDB_FILE:${name}> DESTINATION bin OPTIONAL)
	endif()
endmacro()

macro(mamacpp_binary name)
	add_executable(${name} ${name}.cpp)
	target_link_libraries(${name} mamacpp mama ${ARGN})
	install(TARGETS ${name}
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
	if (WIN32 AND MSVC)
		install(FILES $<TARGET_PDB_FILE:${name}> DESTINATION bin OPTIONAL)
	endif()
endmacro()
