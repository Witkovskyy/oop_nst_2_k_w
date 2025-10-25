#include <iostream>
#include "components/board.hpp"
#include "engine/val.h"
#include "engine/evalpos.cpp"
#include "engine/see.cpp"


int main() {
    using std::cout;
    using std::endl;

    cout << "Chess Engine — minimal runner" << endl;

    // Construct an empty board (all cells default to EMPTY)
    Board b;

    // Print static evaluation of the starting (empty) board
    int material = eval(b);
    cout << "eval(board) = " << material << " (centipawns)" << endl;

    // Run a tiny search using the engine's negamax with fixed depth from val.h
    int score = negamax(b, depth, -INF, INF);
    cout << "negamax(...) = " << score << " (centipawns)" << endl;

    return 0;
}