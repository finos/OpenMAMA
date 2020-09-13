csharp_build_project(MamaChurn load/MamaChurn/MamaChurnCS.csproj MamaChurn mamacs)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MamaChurn.exe DESTINATION bin)
