#pragma once
#include "../main.cpp"
#include <limits>

static inline int pieceValFromSymbol(char symbol)
{
    symbol = toupper(symbol);
    switch (symbol)
    {
    case 'P': // Pawn
        return 100;
    case 'N': // Knight
        return 320;
    case 'B': // Bishop
        return 350;
    case 'R': // Rook
        return 500;
    case 'Q': // Queen
        return 1500;
    case 'K': // King
        return 20000;
    default:
        return 0; // Empty or unknown
    }
}

static const int INF = std::numeric_limits<int>::max();

static const int depth = 5; // Fixed search depth, might change later
