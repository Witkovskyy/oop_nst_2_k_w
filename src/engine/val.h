#pragma once
#include "../components/piece.hpp"
#include <limits>

// Piece values in centipawns
    static const PieceType pieceTypes[] = {
        PieceType::EMPTY,
        PieceType::PAWN,
        PieceType::KNIGHT,
        PieceType::BISHOP,
        PieceType::ROOK,
        PieceType::QUEEN,
        PieceType::KING};

    static const int pieceValues[] = {
        0, 100, 320, 350, 500, 1500, 20000};

    static const int INF = std::numeric_limits<int>::max(); 

    static const int depth = 5;  // Fixed search depth, might change later

