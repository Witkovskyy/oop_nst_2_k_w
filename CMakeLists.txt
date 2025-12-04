cmake_minimum_required(VERSION 3.22)
project(oop_cpp_demo CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#add_library(engine STATIC
 # src/engine/evalpos.cpp
  #src/engine/see.cpp
#)

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
 src/engine/moves.cpp)

#add_executable(tests
 # src/tests/test_engine.cpp
  #src/Piece.cpp
#)

# target_link_libraries(tests PRIVATE engine)

# Don't change these two lines
find_package(SQLite3 REQUIRED)
find_package(SFML 2.6.2 EXACT COMPONENTS graphics window system audio REQUIRED)




target_link_libraries(app 
  PRIVATE 
   # engine 
    SQLite::SQLite3 
    sfml-graphics
    sfml-window 
    sfml-system 
    sfml-audio
)
#enable_testing()
#add_test(NAME engine_tests COMMAND tests)
add_executable(sfml_check src/sfml_check.cpp)
target_link_libraries(sfml_check PRIVATE sfml-graphics sfml-window sfml-system)

add_custom_command(TARGET app POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:app>/pieces
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/src/pieces
          $<TARGET_FILE_DIR:app>/pieces
)
