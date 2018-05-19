cmake_minimum_required (VERSION 2.8.3)
project (mamacs)

msbuild_project(NUnitTest NUnitTest.csproj NUnitTest)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/NUnitTest.dll DESTINATION bin RENAME DOTNET_UNITTESTS.dll)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/nunit.framework.dll DESTINATION bin)