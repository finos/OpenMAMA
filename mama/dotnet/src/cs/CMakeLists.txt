cmake_minimum_required (VERSION 2.8.3)
project (mamacs)

if (WIN32 AND MSVC)
    csharp_build_project(mamacs mamadotnet.csproj MAMA mama)
else ()
    csharp_build_project(mamacs mamadotnetcore.csproj MAMA mama)
endif ()

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MAMA.dll DESTINATION bin)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MAMA.pdb DESTINATION bin OPTIONAL)
