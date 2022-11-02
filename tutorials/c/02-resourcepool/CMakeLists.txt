cmake_minimum_required (VERSION 3.10)
project ("02-resourcepool")

find_package(OpenMAMA REQUIRED)

add_executable(resourcepoolc resourcepool.c)
target_link_libraries(resourcepoolc PRIVATE OpenMAMA::mama)
