cmake_minimum_required(VERSION 3.10)
project(ExprCalc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 源文件目录
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(SOURCES
        ${SOURCE_DIR}/lexer.cpp
        ${SOURCE_DIR}/shunting_yard.cpp
        ${SOURCE_DIR}/evaluator.cpp
        ${SOURCE_DIR}/symbol_table.cpp
        ${SOURCE_DIR}/calculator.cpp
        ${SOURCE_DIR}/logger.cpp
)
set(HEADERS
        ${SOURCE_DIR}/token.h
        ${SOURCE_DIR}/lexer.h
        ${SOURCE_DIR}/shunting_yard.h
        ${SOURCE_DIR}/evaluator.h
        ${SOURCE_DIR}/symbol_table.h
        ${SOURCE_DIR}/calculator.h
        ${SOURCE_DIR}/error.h
        ${SOURCE_DIR}/logger.h
)

# 主程序
add_executable(ExprCalc main.cpp ${SOURCES} ${HEADERS})
target_include_directories(ExprCalc PRIVATE ${SOURCE_DIR})

# GoogleTest 配置
find_package(GTest CONFIG REQUIRED)
enable_testing()
set(TEST_SOURCES
        ${CMAKE_SOURCE_DIR}/tests/test_lexer.cpp
        ${CMAKE_SOURCE_DIR}/tests/test_calculator.cpp
)
add_executable(ExprCalcTests ${TEST_SOURCES} ${SOURCES} ${HEADERS})
target_include_directories(ExprCalcTests PRIVATE ${SOURCE_DIR})
target_link_libraries(ExprCalcTests PRIVATE GTest::gtest GTest::gtest_main)
add_test(NAME ExprCalcTests COMMAND ExprCalcTests)