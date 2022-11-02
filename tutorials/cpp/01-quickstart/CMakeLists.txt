cmake_minimum_required (VERSION 3.10)
project ("01-quickstart")

find_package(OpenMAMA REQUIRED)

add_executable(quickstartcpp quickstart.cpp)
target_link_libraries(quickstartcpp PRIVATE OpenMAMA::wombatcommon OpenMAMA::mama OpenMAMA::mamacpp)