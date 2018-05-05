cmake_minimum_required (VERSION 2.8.3)
project (mamdajava)

find_package(Java)
include(ExternalProject)

if (WIN32)
    find_program(BIN_GRADLE gradle)
else()
    find_program(BIN_GRADLE gradle)
endif()

set(MAMAJNI_JAVA_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/java_build")

ExternalProject_Add(mamdajava
        SOURCE_DIR "."
        BUILD_COMMAND ""
        CONFIGURE_COMMAND ${BIN_GRADLE} --no-daemon -PbuildDir=${MAMAJNI_JAVA_BUILD_DIR} -b ${CMAKE_CURRENT_SOURCE_DIR}/build.gradle jar
        INSTALL_COMMAND ""
        LOG_BUILD 1)
