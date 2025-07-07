cmake_minimum_required(VERSION 3.18)

project(MyAwesomeProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(my_app main.cpp)

# 关键部分：让 CMake 通过 Vcpkg 查找依赖
find_package(fmt CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE fmt::fmt)
