cmake_minimum_required (VERSION 3.10)
project (OpenMAMA)

set (CMAKE_CXX_STANDARD 11)

enable_testing()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(${CMAKE_C_COMPILER_ID})

# Set up default cmake build type if not defined
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "The cmake build type to perform")
  message(STATUS "No CMAKE_BUILD_TYPE provided - defaulting to ${CMAKE_BUILD_TYPE}")
else()
  message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} provided")
endif()

# Build type options
set (WITH_EXAMPLES      true   CACHE BOOL   "Include compiled example applications in the build (e.g. mamalistenc)")
set (WITH_TESTTOOLS     true   CACHE BOOL   "Include test tools in the build (e.g. capturereplay)")
set (WITH_UNITTEST      false  CACHE BOOL   "Include unit tests in the build (e.g. UnitTestMamaC)")
set (WITH_PROTON        true   CACHE BOOL   "Include the qpid proton middleware and payload bridges in the build")
set (WITH_NOOP_BRIDGE   false  CACHE BOOL   "Include the noop example middleware bridge in the build")
set (WITH_BASE_BRIDGE   true   CACHE BOOL   "Include the base bridge in the build (required for qpid proton)")
set (WITH_OMPB_BRIDGE   true   CACHE BOOL   "Include the OpenMAMA Playback bridge in the build (subscriber only)")
set (WITH_MAMDA         true   CACHE BOOL   "Include MAMDA in the build")
set (WITH_CPP           true   CACHE BOOL   "Include C++ bindings")
set (WITH_CSHARP        false  CACHE BOOL   "Include C# bindings")
set (WITH_JAVA          false  CACHE BOOL   "Include JAVA bindings")
set (WITH_DOC           false  CACHE BOOL   "Include documentation")
set (WITH_TUTORIALS     false  CACHE BOOL   "Include building of tutorial code")
set (ENTITLEMENTS       "noop" CACHE STRING "Comma delimited list of entitlement bridges to allow to load at runtime")

# Dependency detection options
set (LIBEVENT_ROOT "${DEFAULT_LIBEVENT_ROOT}" CACHE PATH "Libevent dependency install directory (for qpid)")
set (PROTON_ROOT   "${DEFAULT_PROTON_ROOT}"   CACHE PATH "Qpid dependency install directory (for qpid middleware and payload bridges)")
set (APR_ROOT      "${DEFAULT_APR_ROOT}"      CACHE PATH "Apache APR dependency install directory (for core MAMA)")
set (APRUTIL_ROOT  "${DEFAULT_APRUTIL_ROOT}"  CACHE PATH "Apache APR Utils dependency install directory (for core MAMA)")
set (GTEST_ROOT    "${DEFAULT_GTEST_ROOT}"    CACHE PATH "Google test framework dependency install directory (for unit tests)")
set (OEA_ROOT      "${DEFAULT_OEA_ROOT}"      CACHE PATH "OEA dependency install directory (for ef ENTITLEMENTS is set to oea)")

# Install / post-compilation options
set (INSTALL_RUNTIME_DEPENDENCIES true CACHE BOOL "Whether or not to include dependent .so or .dll files in the installation package")

option(ENABLE_ASAN "Build with address sanitizer" OFF)
if(ENABLE_ASAN)
  message(STATUS "Instrumenting with Address Sanitizer")
  add_definitions("-DENABLE_ASAN")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address -fsanitize-address-use-after-scope")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize-address-use-after-scope")
endif()

option(ENABLE_TSAN "Build with thread sanitizer" OFF)
if(ENABLE_TSAN)
  message(STATUS "Instrumenting with Thread Sanitizer")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer -fsanitize=thread -fPIE")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=thread -fPIE")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=thread")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread -pie")
endif()

option(ENABLE_UBSAN "Build with undefined behavior sanitizer" OFF)
if(ENABLE_UBSAN)
  message(STATUS "Instrumenting with Undefined Behavior Sanitizer")
  set(CMAKE_BUILD_TYPE "Debug")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fno-omit-frame-pointer")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fsanitize=undefined")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fsanitize=implicit-conversion")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fsanitize=implicit-integer-truncation")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fsanitize=integer")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fsanitize=nullability")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fsanitize=vptr")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${UBSAN_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${UBSAN_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${UBSAN_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${UBSAN_FLAGS}")
endif()

add_subdirectory (common)
add_subdirectory (mama)

if (WITH_MAMDA AND WITH_CPP)
  add_subdirectory (mamda)
endif()

if (WITH_CSHARP)
    include (ExternalProject)
    find_program(BIN_DOTNET dotnet)
    message(STATUS "Using dotnet: ${BIN_DOTNET}")
    ExternalProject_Add(OpenMAMA-csharp
        SOURCE_DIR "."
        BUILD_COMMAND ""
	CONFIGURE_COMMAND ${BIN_DOTNET} build -c Release /p:Platform="Any CPU" /p:IsInOpenMAMABuild=true -o ${CMAKE_CURRENT_BINARY_DIR}/OpenMAMA-csharp ${CMAKE_CURRENT_SOURCE_DIR}/OpenMAMA-csharp.sln
        INSTALL_COMMAND ""
        LOG_BUILD 1
        DEPENDS mama)

    install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/OpenMAMA-csharp/
        DESTINATION bin
        FILES_MATCHING 
        REGEX "^.*.exe$"
        REGEX "^.*.dll$"
        REGEX "^.*[0-9].*$" EXCLUDE
    )

    if (WITH_UNITTEST AND NOT IS_MSVC_32_BIT)
        enable_testing()
        add_test(NAME csharp_unittests COMMAND ${BIN_DOTNET} ${CMAKE_CURRENT_BINARY_DIR}/OpenMAMA-csharp/mamaunittestscs.dll)
        set_tests_properties(csharp_unittests
                             PROPERTIES
                                 ENVIRONMENT "middlewareName=qpid;transportName=pub")
    endif()
endif()

if (WITH_JAVA)
    include(ExternalProject)
    find_package(Java)

    if (WIN32)
        find_program(BIN_GRADLE gradlew.bat HINTS ${CMAKE_SOURCE_DIR})
    else()
        find_program(BIN_GRADLE gradlew HINTS ${CMAKE_SOURCE_DIR})
    endif()

    message(STATUS "Using gradle: ${BIN_GRADLE}")

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/OpenMAMA-java/mamajni)

    ExternalProject_Add(OpenMAMA-java
        BUILD_IN_SOURCE 1
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
        BUILD_COMMAND ""
        CONFIGURE_COMMAND
            ${BIN_GRADLE} --no-daemon -PbuildDir=${CMAKE_CURRENT_BINARY_DIR}/OpenMAMA-java -PmamaJniHeaderDir=${CMAKE_BINARY_DIR}/OpenMAMA-java/mamajni
                jar testClasses
        INSTALL_COMMAND ""
        LOG_BUILD 1
        DEPENDS mama)

    install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/OpenMAMA-java/libs/
        DESTINATION lib
        FILES_MATCHING
        REGEX "^.*.jar$"
    )

    install (DIRECTORY mama/jni/src/main/java/com/wombat/mama/examples/ DESTINATION examples/mamajni)
    install (DIRECTORY mamda/java/src/main/java/com/wombat/mamda/examples/ DESTINATION examples/com/wombat/mamda/examples)

    message(STATUS "Will use data dictionary: ${CMAKE_SOURCE_DIR}/mamda/java/src/test/resources/data.dict")

    if (WITH_UNITTEST)
        enable_testing()
        add_test(NAME java_unittests
                 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                 COMMAND ${BIN_GRADLE} --no-daemon
                     -PbuildDir=${CMAKE_CURRENT_BINARY_DIR}/OpenMAMA-java
                     -Ddictionary=${CMAKE_SOURCE_DIR}/mamda/java/src/test/resources/data.dict
                     test -i)
        set_tests_properties(java_unittests
                             PROPERTIES
                                 ENVIRONMENT "middlewareName=qpid;transportName=pub")
    endif()
endif()

if (WITH_TUTORIALS)
  add_subdirectory(tutorials/c)
  if (WITH_CPP)
    add_subdirectory(tutorials/cpp)
  endif()
endif()

install(FILES LICENSE.md DESTINATION .)
install(FILES mama/c_cpp/src/examples/mama.properties DESTINATION config)

include(CPack)
