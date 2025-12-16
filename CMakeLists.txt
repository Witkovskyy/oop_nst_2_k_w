cmake_minimum_required(VERSION 3.22)
project(oop_cpp_demo CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Engine library
add_library(engine STATIC
  src/engine/evalpos.cpp
  src/engine/see.cpp
  src/engine/moves.cpp
  src/engine/tables/zobrist.cpp
)
target_include_directories(engine PUBLIC src/engine)

# Don't change these lines
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)
find_package(SQLite3 REQUIRED)
find_package(SFML 2.6.2 EXACT COMPONENTS graphics window system audio REQUIRED)

# Application executable
add_executable(app
  src/main.cpp
  src/Board.cpp
  src/Piece.cpp
  src/Bishop.cpp
  src/Rook.cpp
  src/King.cpp
  src/Pawn.cpp
  src/Queen.cpp
  src/Piece.cpp
  src/kNight.cpp
)

target_link_libraries(app 
  PRIVATE 
    engine 
    SQLite::SQLite3 
    sfml-graphics
    sfml-window 
    sfml-system 
    sfml-audio
)
# Copy pieces directory to output directory after build
add_custom_command(TARGET app POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:app>/pieces
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/src/pieces
          $<TARGET_FILE_DIR:app>/pieces
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${CMAKE_SOURCE_DIR}/src/arial.ttf
          $<TARGET_FILE_DIR:app>/arial.ttf
)

# Tests executable

add_executable(tests
  src/tests/catch2.cpp
  src/Board.cpp
  src/Piece.cpp
  src/Bishop.cpp
  src/Rook.cpp
  src/King.cpp
  src/Pawn.cpp
  src/Queen.cpp
  src/kNight.cpp
)

target_link_libraries(tests 
    PRIVATE 
    Catch2::Catch2WithMain
    engine
    sfml-graphics
    sfml-window
    sfml-system
)

target_include_directories(tests PRIVATE src)

include(CTest)
include(Catch)
catch_discover_tests(tests)

#Other executables
add_executable(sfml_check src/sfml_check.cpp)
target_link_libraries(sfml_check PRIVATE sfml-graphics sfml-window sfml-system)