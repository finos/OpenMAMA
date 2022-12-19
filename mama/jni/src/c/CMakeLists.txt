project (mamajnic)

get_target_property(MAMA_INCLUDES mama INCLUDE_DIRECTORIES)

configure_file(version.c.in "${CMAKE_CURRENT_BINARY_DIR}/version.c" @ONLY)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	add_definitions(-Wno-deprecated-declarations -Wno-pointer-sign)
endif()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${JNI_INCLUDE_DIRS}
	${MAMA_INCLUDES}
	${CMAKE_BINARY_DIR}/OpenMAMA-java
)

set(sources 
	${CMAKE_CURRENT_BINARY_DIR}/version.c
	mamajni.c
	mamaconnectionjni.c
	mamatransportjni.c
	mamabridgejni.c
	mamasubscriptionjni.c
	mamabasicsubscriptionjni.c
	mamamsgjni.c
	mamamsgfieldjni.c
	mamatimerjni.c
	mamadictionaryjni.c
	mamaqueuejni.c
	mamadispatcherjni.c
	mamapublisherjni.c
	mamadqpublisherjni.c
	mamadqpublishermanagerjni.c
	mamainboxjni.c
	mamajniutils.c
	mamapricejni.c
	mamadatetimejni.c
	mamasourcejni.c
	mamaftmemberjni.c
	subscriptioncommon.c
	mamaloghandler.c
	mamapayloadbridgejni.c
)

add_library(mamajni SHARED ${sources})
target_link_libraries(mamajni mama)
add_dependencies(mamajni OpenMAMA-java)

set_target_properties(mamajni PROPERTIES OUTPUT_NAME "mamajni${OPENMAMA_MAMAJNI_TARGET_SUFFIX}")

if (WIN32)
    set_target_properties(mamajni PROPERTIES PREFIX "")
endif()

install(TARGETS mamajni
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:mamajni> DESTINATION bin OPTIONAL)
endif()
