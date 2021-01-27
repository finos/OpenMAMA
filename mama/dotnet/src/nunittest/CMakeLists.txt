cmake_minimum_required (VERSION 2.8.3)
project (mamacs)

enable_testing()

csharp_build_project(NUnitTest NUnitTest.csproj NUnitTest mamacs)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/NUnitTest.dll DESTINATION bin RENAME DOTNET_UNITTESTS.dll)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/nunit.framework.dll DESTINATION bin)

add_test(NAME mama_csharp_nunit
         COMMAND nunit-console.exe ${CMAKE_CURRENT_BINARY_DIR}/NUnitTest.dll)
set_tests_properties(mama_csharp_nunit
                     PROPERTIES
                         ENVIRONMENT "middlewareName=qpid;transportName=pub"
)