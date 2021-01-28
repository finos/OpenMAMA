project (mamdajava)

find_package(Java)
include(ExternalProject)

if (WIN32)
    find_program(BIN_GRADLE gradle)
else()
    find_program(BIN_GRADLE gradle)
endif()

set(MAMAJNI_JAVA_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/mamda_java_build")

list(APPEND GRADLE_TARGETS jar)
if(WITH_DOC)
	list(APPEND GRADLE_TARGETS javadoc)
	set(GRADLE_EXTRA_ARGS "-PmamdaVersion=${MAMDA_VERSION_MAJOR}.${MAMDA_VERSION_MINOR}.${MAMDA_VERSION_RELEASE}")
	install(DIRECTORY ${MAMAJNI_JAVA_BUILD_DIR}/docs/javadoc/ DESTINATION doc/mamda/java/html)
endif()

ExternalProject_Add(mamdajava
        SOURCE_DIR "."
        BUILD_COMMAND ""
        CONFIGURE_COMMAND ${BIN_GRADLE} --no-daemon -PlegacyNaming -PbuildDir=${MAMAJNI_JAVA_BUILD_DIR} -b ${CMAKE_CURRENT_SOURCE_DIR}/build.gradle ${GRADLE_EXTRA_ARGS} ${GRADLE_TARGETS}
        INSTALL_COMMAND ""
        LOG_BUILD 1)
add_dependencies(mamdajava mamajnijava)
install (FILES ${MAMAJNI_JAVA_BUILD_DIR}/libs/mamda.jar DESTINATION lib)

install(DIRECTORY src/main/java/com/wombat/mamda/examples/ DESTINATION examples/com/wombat/mamda/examples)
