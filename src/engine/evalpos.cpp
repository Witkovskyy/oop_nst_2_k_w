#pragma once
#include "../components/board.hpp"
#include "../components/piece.hpp"
#include "helpers.hpp"
#include "val.h"
#include <algorithm>
#include <vector>
#include <limits>

// Simple instrumentation: count nodes visited by negamax
static long nodesVisited = 0;
long get_nodes_visited() { return nodesVisited; }

int eval(const Board &b)
{

    // Simple material evaluation
    int score = 0;
    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            const Cell &cell = b.cells[r][c];
            if (cell.piece == PieceType::EMPTY)
                continue;
            int idx = static_cast<int>(cell.piece);
            if (idx < 0 || idx > 6)
                continue;
            int v = pieceValues[idx];
            if (cell.color == Color::WHITE)
                score += v;
            else
                score -= v;
        }
    }
    return score;
}

// Prototypes for negamax and eval
// Forward declarations for functions assumed to exist

// Quiescence search to avoid horizon effect
int quiescence(Board& b, int alpha, int beta) {
    int stand = eval(b);
    if (stand >= beta) return beta;
    if (stand > alpha) alpha = stand;

    auto caps = generateCaptures(b);     // Only consider capture moves
    for (auto& m : caps) {
        Undo u; applyMove(b,m,u);
        int score = -quiescence(b, -beta, -alpha);
        undoMove(b,m,u);
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

// Negamax algorithm with alpha-beta pruning
// Cutting off branches that won't influence the final decision
int negamax(Board& b, int depth, int alpha, int beta) {
    ++nodesVisited;
    if (depth == 0 || gameOver(b))
        return quiescence(b, alpha, beta);

    int best = -INF;
    auto moves = legalMoves(b);
    if (moves.empty())             // No legal moves
        return quiescence(b, alpha, beta);

    orderMoves(moves);                   
    for (auto& m : moves) {
        Undo u; applyMove(b,m,u);
        int score = -negamax(b, depth-1, -beta, -alpha);
        undoMove(b,m,u);
        if (score > best) best = score;
        if (score > alpha) alpha = score;
        if (alpha >= beta) break;        // beta cutoff
    }
    return best;
}
