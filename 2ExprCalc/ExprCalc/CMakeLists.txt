cmake_minimum_required(VERSION 3.10)
project(ExprCalc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 源文件目录
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(SOURCES
        ${SOURCE_DIR}/lexer.cpp
)
set(HEADERS
        ${SOURCE_DIR}/token.h
        ${SOURCE_DIR}/lexer.h
)

# 可执行目标
add_executable(ExprCalc main.cpp ${SOURCES} ${HEADERS})
target_include_directories(ExprCalc PRIVATE ${SOURCE_DIR})