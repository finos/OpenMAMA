enable_testing()
include_directories(${GTEST_INCLUDE_DIRS})

get_target_property(WOMBAT_INCLUDES wombatcommon INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${WOMBAT_INCLUDES}
	..
)

add_executable(UnitTestCommonC
	../MainUnitTestC.cpp
	../MainUnitTestC.h
	port.cpp
	queuetest.cpp
	strutilstest.cpp
	timertest.cpp
	utiltest.cpp
)

if (WIN32)
    target_link_libraries(UnitTestCommonC ${GTEST_BOTH_LIBRARIES} wombatcommon)
else()
    target_link_libraries(UnitTestCommonC ${GTEST_BOTH_LIBRARIES} wombatcommon pthread dl)
endif()

if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:UnitTestCommonC> DESTINATION bin OPTIONAL)
endif()

install(TARGETS UnitTestCommonC
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# Install any dependant libraries
if (INSTALL_RUNTIME_DEPENDENCIES)
	if (GTEST_ROOT)
		if (WIN32)
			install (DIRECTORY ${GTEST_ROOT}/lib/ DESTINATION bin FILES_MATCHING PATTERN "*gtest*.dll")
			install (DIRECTORY ${GTEST_ROOT}/bin/ DESTINATION bin FILES_MATCHING PATTERN "*gtest*.dll")
		endif()
	endif()
endif()
