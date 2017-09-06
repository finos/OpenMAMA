cmake_minimum_required (VERSION 2.8)
project (mamac)

list(APPEND CMAKE_MODULE_PATH
	    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake
	    ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake
)

include(mama-common)
include(${CMAKE_C_COMPILER_ID})
include(FindAPR)

find_package(APR)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${WOMBAT_INCLUDES}
	${APR_INCLUDE_DIR}
)

configure_file(version.h.in "${CMAKE_CURRENT_BINARY_DIR}/version.h" @ONLY)
configure_file(version.c.in "${CMAKE_CURRENT_BINARY_DIR}/version.c" @ONLY)

set(sources
	bridge.c
	clientmanageresponder.c
	datetime.c
	dictionary.c
	dqpublisher.c
	dqpublishermanager.c
	dqstrategy.c
	entitlement.c
	entitlementlibraries.c
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
)

list(APPEND sources ${SYSTEM_SOURCES})

add_library(mamac SHARED ${sources})
