cmake_minimum_required(VERSION 2.8)
set(CMAKE_C_FLAGS "-std=c99")
set(CMAKE_CXX_FLAGS "-std=c++11")

project("wi-proxy")

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/${PROJECT_NAME}/*.h" "include/${PROJECT_NAME}/*.hpp")

add_executable("${PROJECT_NAME}" ${SOURCES} ${HEADERS})
target_include_directories("${PROJECT_NAME}" PUBLIC "include/${PROJECT_NAME}/")