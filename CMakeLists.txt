cmake_minimum_required(VERSION 3.22)
project(oop_cpp_demo CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(engine STATIC
  src/engine/evalpos.cpp
  src/engine/see.cpp
)

add_executable(app
  src/main.cpp
  src/Piece.cpp
)

add_executable(tests
  src/tests/test_engine.cpp
  src/Piece.cpp
)

target_link_libraries(tests PRIVATE engine)

enable_testing()
add_test(NAME engine_tests COMMAND tests)

find_package(SQLite3 REQUIRED)
find_package(SFML 2.6.2 COMPONENTS graphics window system audio REQUIRED)
target_link_libraries(app 
  PRIVATE 
    engine 
    SQLite::SQLite3 
    sfml-graphics
    sfml-window 
    sfml-system 
    sfml-audio)
