#pragma once
#include <random>
#include <vector>

// 12 Pieces = 6 white, 6 black * 64 pola
// White: P=0, N=1, B=2, R=3, Q=4, K=5
// Black: P=6, N=7, B=8, R=9, Q=10, K=11
// Index = pieceType + (color * 6)
extern unsigned long long pieceKeys[12][64];
extern unsigned long long sideKey; // Key for each side to move

// Initialize Zobrist keys
inline void initZobrist() {
    std::mt19937_64 rng(12345); // Static seed for reproducibility

    for (int p = 0; p < 12; ++p) {
        for (int s = 0; s < 64; ++s) {
            pieceKeys[p][s] = rng();
        }
    }
    sideKey = rng();
}

// Mapping helper
inline int getPieceIndex(char symbol, int color) {
    symbol = toupper(symbol);
    int index = 0;
    switch (symbol) {
    case 'P': index = 0; break;
    case 'N': index = 1; break;
    case 'B': index = 2; break;
    case 'R': index = 3; break;
    case 'Q': index = 4; break;
    case 'K': index = 5; break;
    }
    if (color == 1) index += 6; // Move black +6
    return index;
}