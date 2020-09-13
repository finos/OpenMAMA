if (WIN32 AND MSVC)
    csharp_build_project(mamdacs mamdadotnet.csproj MAMDA mamacs)
else ()
    csharp_build_project(mamdacs mamdadotnetcore.csproj MAMDA mamacs)
endif ()

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MAMDA.dll DESTINATION bin)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MAMDA.pdb DESTINATION bin OPTIONAL)
