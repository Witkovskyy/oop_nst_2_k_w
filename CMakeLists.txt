cmake_minimum_required(VERSION 3.22)
project(oop_cpp_demo CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(app src/main.cpp)

find_package(SQLite3 REQUIRED)
target_link_libraries(app PRIVATE SQLite::SQLite3)
