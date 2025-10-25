#pragma once
#include "../components/board.hpp"
#include "../components/piece.hpp"
#include "val.h"
#include <algorithm>
#include <vector>
#include <limits>
#include <cstdlib>


static inline bool inBounds(int r, int c) { return (unsigned)r < 8 && (unsigned)c < 8; }
static inline Color opp(Color c) { return c == Color::WHITE ? Color::BLACK : Color::WHITE; }

static bool lineClear(const Board& b, Square a, Square d) {
    int dr = (d.r > a.r) - (d.r < a.r);
    int dc = (d.c > a.c) - (d.c < a.c);
    int r = a.r + dr, c = a.c + dc;
    while (r != d.r || c != d.c) {
        if (b.cells[r][c].piece != PieceType::EMPTY) return false;
        r += dr; c += dc;
    }
    return true;
}

// Can the piece from 'from' attack square 'to' (does NOT check move legality, only geometry/lines)
static bool canAttack(const Board& b, Square from, Square to) {
    if (from.r == to.r && from.c == to.c) return false;
    const Cell &src = b.cells[from.r][from.c];
    switch (src.piece) {
        case PieceType::PAWN: {
            int dir = (src.color == Color::WHITE) ? -1 : +1;
            int rr = from.r + dir;
            return (to.r == rr && (to.c == from.c - 1 || to.c == from.c + 1));
        }
        case PieceType::KNIGHT: {
            int dr = std::abs(from.r - to.r), dc = std::abs(from.c - to.c);
            return (dr == 1 && dc == 2) || (dr == 2 && dc == 1);
        }
        case PieceType::BISHOP:
            return std::abs(from.r - to.r) == std::abs(from.c - to.c) && lineClear(b, from, to);
        case PieceType::ROOK:
            return (from.r == to.r || from.c == to.c) && lineClear(b, from, to);
        case PieceType::QUEEN:
            return ((from.r == to.r || from.c == to.c) || (std::abs(from.r - to.r) == std::abs(from.c - to.c)))
                   && lineClear(b, from, to);
        case PieceType::KING:
            return std::max(std::abs(from.r - to.r), std::abs(from.c - to.c)) == 1;
        default:
            return false;
    }
}

static int pieceVal(PieceType p) { return pieceValues[static_cast<int>(p)]; }

// Returns the cheapest attacker of the given color on 'sq' in position 'b'. If none, r=-1.
static Square cheapestAttacker(const Board& b, Square sq, Color side) {
    Square best{-1,-1};
    int bestVal = 1e9;
    for (int r=0; r<8; ++r) for (int c=0; c<8; ++c) {
        const auto& cell = b.cells[r][c];
        if (cell.piece == PieceType::EMPTY || cell.color != side) continue;
        if (!canAttack(b, {r,c}, sq)) continue;
        int v = pieceVal(cell.piece);
        if (v < bestVal) { bestVal = v; best = {r,c}; }
    }
    return best;
}

// SEE: positive result = gain for 'sideToMove' initiating the exchange on 'target'.
int see(const Board& start, Square target, Color sideToMove) {
    const Cell &victimCell = start.cells[target.r][target.c];
    if (victimCell.piece == PieceType::EMPTY) return 0;

    // Copy of the board for a 'static' simulation of captures on a single square
    Board b = start;

    // Gains at successive depths (basic version of Tord Romstad's SEE algorithm)
    int gain[32]; int d = 0;
    gain[0] = pieceVal(victimCell.piece);

    Color stm = sideToMove;
    PieceType captured = victimCell.piece;
    Square to = target;

    // Repeat: the cheapest attacker of side 'stm' captures on 'to'
    while (true) {
        Square a = cheapestAttacker(b, to, stm);
        if (a.r == -1) break; // no further captures

        const Cell attacker = b.cells[a.r][a.c];
        const int attackerVal = pieceVal(attacker.piece);

        // prepare gain for the next depth: the attacker becomes the new victim
        int next = attackerVal - gain[d];
        gain[++d] = next;

        // perform a static 'capture' on the board copy:
        b.cells[a.r][a.c] = Cell{};                  // empty square left by the attacker
        b.cells[to.r][to.c] = { attacker.piece, attacker.color }; // attacker moves onto the square

        // new situation: the other side will capture the piece that just captured
        stm = opp(stm);
        captured = attacker.piece;
        // 'to' (the exchange square) does not change — captures continue on the same square
    }

    // Minimax from the end (if the opponent can worsen the result, it will)
    for (int i = d - 1; i >= 0; --i)
        gain[i] = std::max(-gain[i+1], gain[i]);

    return gain[0];
}
