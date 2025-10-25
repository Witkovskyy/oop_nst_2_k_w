#pragma once
#include "piece.hpp"

struct Square {
    int r;
    int c;
};

struct Cell {
    PieceType piece{PieceType::EMPTY};
    Color color{Color::NONE};
};

struct Undo {
    // Simple undo information: store source and destination cells and coords
    Cell srcCell;
    Cell destCell;
    int from_r{0}, from_c{0};
    int to_r{0}, to_c{0};
};

struct Board{
public:
    Cell cells[8][8];
    // other board state can be added later
};