msbuild_project(mamdacs mamdadotnet.csproj MAMDA)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MAMDA.dll DESTINATION bin)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MAMDA.pdb DESTINATION bin OPTIONAL)
