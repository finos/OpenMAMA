cmake_minimum_required (VERSION 3.10)
project ("01-quickstart")

find_package(OpenMAMA REQUIRED)

add_executable(quickstartc quickstart.c)
target_link_libraries(quickstartc PRIVATE OpenMAMA::wombatcommon OpenMAMA::mama OpenMAMA::mamacpp)
