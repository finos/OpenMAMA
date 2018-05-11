msbuild_project(MamaChurn load/MamaChurn/MamaChurnCS.csproj MamaChurn)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MamaChurn.exe DESTINATION bin)
