cmake_minimum_required(VERSION 2.8)
set(CMAKE_C_VERSION 99)
set(CMAKE_CXX_VERSION 11)

project("web-infrastructure-assignment-1")

file(GLOB MY_SOURCES "src/*.cpp" "src/*.hpp" "src/*.h" "include/web-infrastructure-assignment-1/*.h" "include/web-infrastructure-assignment-1/*.hpp")
add_executable("${PROJECT_NAME}" ${MY_SOURCES})