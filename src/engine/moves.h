#pragma once
#include "../Board.h"
#include "../Piece.h"
#include <vector>
struct Move {
    Position from;
    Position to;
    Piece* pieceMoved;
    Piece* pieceCaptured;
};

std::vector<Move> generateAllMoves(Board& board, int color);
std::vector<Move> generateAllCaptures(Board& board, int color);
