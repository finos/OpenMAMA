cmake_minimum_required (VERSION 2.8.3)
project (mamacs)

msbuild_project(mamacs mamadotnet.csproj MAMA)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MAMA.dll DESTINATION bin)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MAMA.pdb DESTINATION bin OPTIONAL)