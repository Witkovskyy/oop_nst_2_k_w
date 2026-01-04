# C++ Chess Engine & GUI

A feature-rich Chess Engine and Graphical User Interface built with **C++17** and **SFML**. This project demonstrates advanced Object-Oriented Programming concepts, custom AI algorithms, and multithreaded architecture.

##  Features

### Core Game Mechanics
* Full implementation of chess rules (movement, captures, check, checkmate, stalemate).
* **Move Validation:** Robust system to prevent illegal moves (e.g., moving into check).

###  AI Engine
The project features a custom-built chess engine capable of playing against a human:
* **Negamax Algorithm:** Simplified minimax variant for move evaluation.
* **Alpha-Beta Pruning:** Optimizes the search tree by eliminating irrelevant branches.
* **Quiescence Search:** Extends the search at leaf nodes to avoid the "horizon effect" during captures.
* **Zobrist Hashing:** Efficient board state hashing for fast lookups.
* **Transposition Table (TT):** Caches search results to avoid re-evaluating the same positions (64MB default size).
* **Evaluation Function:** Uses Material balance and Piece-Square Tables (PST) for positional scoring.
* **Static Exchange Evaluation (SEE):** heuristic to determine if a capture is profitable.

###  Architecture & Design
* **Object-Oriented Design:** Polymorphic `Piece` hierarchy (`Pawn`, `Rook`, `Knight`, etc.).
* **Design Patterns:**
    * **Singleton:** Used for the `AsyncLogger` to ensure a single logging point.
    * **RAII:** Resource management for SFML assets and memory.
* **Multithreading:**
    * **Async Logger:** A dedicated thread processes log messages from a queue to prevent I/O blocking in the main game loop.
    * Thread-safe operations using `std::mutex` and `std::condition_variable`.

###  Tech Stack
* **Language:** C++17
* **Graphics:** SFML (Simple and Fast Multimedia Library) 2.6+
* **Build System:** CMake
* **Testing:** Catch2 (Unit Tests & Coverage)

---

##  Installation & Build

### Prerequisites
* C++ Compiler supporting C++17 (GCC, Clang, or MSVC)
* CMake (3.10 or higher)
* SFML Library (must be installed or provided via vcpkg/package manager)

### Building from Source

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/Witkovskyy/oop_nst_2_k_w.git](https://github.com/Witkovskyy/oop_nst_2_k_w.git)
    cd oop_nst_2_k_w
    ```

2.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

3.  **Run CMake and Compile:**
    ```bash
    cmake ..
    make
    # Or on Windows with MSVC: cmake --build . --config Release
    ```

4.  **Run the Game:**
    ```bash
    ./ChessGame
    ```

---

##  Controls

* **Mouse Left Click:** Select a piece.
* **Mouse Drag & Drop:** Move a piece to a target square.
* **Right Click:** Deselect piece (optional, depending on implementation).

---

##  Testing

The project maintains high code coverage using the **Catch2** framework. The tests cover board logic, piece movement rules, engine evaluation, and Zobrist hashing integrity.

To run the tests:

1.  Build the test executable (usually defined in CMake):
    ```bash
    make tests
    ```
2.  Execute:
    ```bash
    ./tests
    ```

*Sample Test Output:*
```text
-------------------------------------------------------------------------------
Checkmate and Stalemate detection in Negamax
-------------------------------------------------------------------------------
...
All tests passed (87 assertions in 13 test cases)
-------------------------------------------------------------------------------
```
##  Project Structure
```
src/
├── Board.h/cpp           # Chess board logic & state representation
├── Piece.h/cpp           # Base class for all pieces
├── Pieces/               # Derived classes (Rook, Pawn, King, etc.)
├── engine/
│   ├── engine.h/cpp      # Main AI loop (Negamax, Alpha-Beta)
│   ├── evalpos.cpp       # Evaluation function & Attack detection
│   ├── moves.cpp         # Move generation logic
│   ├── tables/           # Transposition Table & Zobrist logic
│   └── logger/           # AsyncLogger implementation
├── main.cpp              # Entry point & SFML Event Loop
└── tests/                # Catch2 unit tests
```