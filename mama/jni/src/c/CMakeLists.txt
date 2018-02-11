cmake_minimum_required (VERSION 2.8)
project (mamajnic)

get_target_property(MAMA_INCLUDES mama INCLUDE_DIRECTORIES)

configure_file(version.c.in "${CMAKE_CURRENT_BINARY_DIR}/version.c" @ONLY)

find_package(JNI)

get_target_property(MAMA_JAVAH_INCLUDES mamajnijava JAVAH_INCLUDES)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	add_definitions(-Wno-deprecated-declarations)
endif()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${JNI_INCLUDE_DIRS}
	${MAMA_INCLUDES}
	${MAMA_JAVAH_INCLUDES}
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
add_dependencies(mamajni mamajnijavah)

install(TARGETS mamajni
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)