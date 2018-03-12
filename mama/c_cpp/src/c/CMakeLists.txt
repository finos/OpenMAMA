cmake_minimum_required (VERSION 2.8)
project (mamac)

list(APPEND CMAKE_MODULE_PATH
	    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake
	    ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake
)

include(${CMAKE_C_COMPILER_ID})
include(FindAPR)

find_package(APR)

get_target_property(WOMBAT_INCLUDES wombatcommon INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${WOMBAT_INCLUDES}
	${APR_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR} # for generated files like version.h etc
)	

set(ENTITLEMENT_BRIDGES "")
if(DEFINED ENTITLEMENTS)
	string(REPLACE "," ";" ENT_LIST ${ENTITLEMENTS})
	foreach(ENT ${ENT_LIST})
		    set(ENTITLEMENT_BRIDGES "${ENTITLEMENT_BRIDGES}\"${ENT}\", ")
	endforeach()
endif()
set(ENTITLEMENT_BRIDGES "${ENTITLEMENT_BRIDGES}NULL")

add_definitions(-DMAMA -DOPENMAMA_INTEGRATION)

configure_file(version.h.in "${CMAKE_CURRENT_BINARY_DIR}/mama/version.h" @ONLY)
configure_file(version.c.in "${CMAKE_CURRENT_BINARY_DIR}/version.c" @ONLY)
configure_file(entitlementlibraries.c.in "${CMAKE_CURRENT_BINARY_DIR}/entitlementlibraries.c" @ONLY)

set(sources
	bridge.c
	clientmanageresponder.c
	datetime.c
	dictionary.c
	dqpublisher.c
	dqpublishermanager.c
	dqstrategy.c
	endpointpool.c
	entitlement.c
	${CMAKE_CURRENT_BINARY_DIR}/entitlementlibraries.c
	error.c
	fielddesc.c
	fileutils.c
	ft.c
	imagerequest.c
	inbox.c
	io.c
	listenermsgcallback.c
	log.c
	mama.c
	mamaStrUtils.c
	mamautils.c
	marketdata.c
	middleware.c
	msg.c
	msgfield.c
	msgqualifier.c
	msgstatus.c
	msgtype.c
	msgutils.c
	plugin.c
	price.c
	priceimpl.c
	publisher.c
	quality.c
	queue.c
	refreshtransport.c
	registerfunctions.c
	reservedfields.c
	senderId.c
	servicelevel.c
	source.c
	sourceman.c
	stat.c
	statfields.c
	statscollector.c
	statsgenerator.c
	statslogger.c
	statsloggerfields.c
	status.c
	subscmsgtype.c
	subscription.c
	subscriptiontype.c
	symbollist.c
	symbollistmember.c
	symbolmapfile.c
	syncresponder.c
	throttle.c
	timer.c
	timezone.c
	transport.c
	${CMAKE_CURRENT_BINARY_DIR}/version.c
	playback/playbackFileParser.c
	playback/playbackpublisher.c
	playback/playbackcapture.c
	conflation/connection.c
	conflation/serverconnection.c
	conflation/manager.c
	fieldcache/fieldcache.c
	fieldcache/fieldcachefieldimpl.c
 	fieldcache/fieldcacheiterator.c
	fieldcache/fieldcachemaparray.c
	fieldcache/fieldcachemap.c
	fieldcache/fieldcacherecord.c
	fieldcache/fieldcachefield.c
	fieldcache/fieldcacheimpl.c
	fieldcache/fieldcachelist.c
	fieldcache/fieldcachevector.c
)

list(APPEND sources ${SYSTEM_SOURCES})

list(APPEND MAMA_LINK_LIBRARIES ${APR_LIBRARY} wombatcommon)
if(UNIX)
	list(APPEND MAMA_LINK_LIBRARIES m)
endif()

add_library(mama SHARED ${sources})
target_link_libraries(mama ${MAMA_LINK_LIBRARIES})

install(TARGETS mama
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY mama/ DESTINATION include/mama)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/mama/version.h" DESTINATION include/mama)

