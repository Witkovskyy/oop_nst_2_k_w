#pragma once

#include <vector>
#include "../components/board.hpp"

// Minimal Move type used by the engine; replace with full Move when available
struct Move {
    int from_r{0}, from_c{0};
    int to_r{0}, to_c{0};
};

// Simple move generator: for every occupied cell, generate up to 4 pseudo-moves
// (up/down/left/right) within the board. This is NOT full chess rules but
// lets the search explore branches so we can verify the engine behavior.
static inline bool in_bounds(int r, int c) {
    return r >= 0 && r < 8 && c >= 0 && c < 8;
}

static inline std::vector<Move> generateCaptures(const Board &b) {
    std::vector<Move> out;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            const Cell &src = b.cells[r][c];
            if (src.piece == PieceType::EMPTY) continue;
            // check 4 directions for captures
            const int dr[4] = {-1,1,0,0};
            const int dc[4] = {0,0,-1,1};
            for (int k = 0; k < 4; ++k) {
                int nr = r + dr[k];
                int nc = c + dc[k];
                if (!in_bounds(nr,nc)) continue;
                const Cell &dst = b.cells[nr][nc];
                if (dst.piece != PieceType::EMPTY && dst.color != src.color) {
                    Move m; m.from_r = r; m.from_c = c; m.to_r = nr; m.to_c = nc;
                    out.push_back(m);
                }
            }
        }
    }
    return out;
}

static inline std::vector<Move> legalMoves(const Board &b) {
    std::vector<Move> out;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            const Cell &src = b.cells[r][c];
            if (src.piece == PieceType::EMPTY) continue;
            // generate up/down/left/right moves into empty squares or captures
            const int dr[4] = {-1,1,0,0};
            const int dc[4] = {0,0,-1,1};
            for (int k = 0; k < 4; ++k) {
                int nr = r + dr[k];
                int nc = c + dc[k];
                if (!in_bounds(nr,nc)) continue;
                const Cell &dst = b.cells[nr][nc];
                if (dst.piece == PieceType::EMPTY || dst.color != src.color) {
                    Move m; m.from_r = r; m.from_c = c; m.to_r = nr; m.to_c = nc;
                    out.push_back(m);
                }
            }
        }
    }
    return out;
}

static inline void orderMoves(std::vector<Move> &moves) {
    // no-op for now
    (void)moves;
}

// applyMove and undoMove implement a reversible local change using Undo
static inline void applyMove(Board &b, const Move &m, Undo &u) {
    u.from_r = m.from_r; u.from_c = m.from_c; u.to_r = m.to_r; u.to_c = m.to_c;
    u.srcCell = b.cells[m.from_r][m.from_c];
    u.destCell = b.cells[m.to_r][m.to_c];

    // move piece
    b.cells[m.to_r][m.to_c] = b.cells[m.from_r][m.from_c];
    // clear source
    b.cells[m.from_r][m.from_c].piece = PieceType::EMPTY;
    b.cells[m.from_r][m.from_c].color = Color::NONE;
}

static inline void undoMove(Board &b, const Move &m, Undo &u) {
    // restore
    b.cells[u.from_r][u.from_c] = u.srcCell;
    b.cells[u.to_r][u.to_c] = u.destCell;
}

static inline bool gameOver(const Board &b) {
    (void)b; return false;
}
