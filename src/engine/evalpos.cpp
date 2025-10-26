#pragma once
#include "../Board.h"
#include "val.h"
#include "deepcopy.h"
#include "moves.h"
#include <algorithm>
#include <vector>
#include <limits>

// count nodes visited by negamax
static long nodesVisited = 0;
long get_nodes_visited() { return nodesVisited; }

int eval(const Board &board)
{
    // Simple material evaluation
    int score = 0;
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            const Piece *cell = board.squares[row][col];
            if (cell == nullptr)
                continue;
            if (cell->symbol == 'E')
                continue;
            int value = pieceValFromSymbol(cell->symbol);
            if (cell->color == 0) // Assuming 0 represents WHITE
                score += value;
            else
                score -= value;
        }
    }
    return score;
}


static std::vector<Move> legalMoves(const Board &board, int color)
{
    // reuse existing generator in moves.h
    return generateAllMoves(const_cast<Board &>(board), color);
}

static std::vector<Move> generateCaptures(Board &board, int color)
{
    return generateAllCaptures(board);
}

struct Undo
{
    Position from;
    Position to;
    Piece *pieceMoved;
    Piece *pieceCaptured;
};

static void undoMove(Board &board, const Move &move, const Undo &undo)
{
    board.squares[move.from.row][move.from.col] = undo.pieceMoved;
    board.squares[move.to.row][move.to.col] = undo.pieceCaptured;
}

static void applyMove(Board &board, const Move &move, Undo &undo)
{
    undo.from = move.from;
    undo.to = move.to;
    undo.pieceMoved = board.getPieceAt(move.from);
    undo.pieceCaptured = board.getPieceAt(move.to);
    board.movePiece(move.from, move.to, undo.pieceMoved);
}

static void orderMoves(std::vector<Move> &moves)
{
    // no-op for now
    (void)moves;
}

// game over if one king is missing
static bool gameOver(const Board &board)
{
    bool whiteKing = false, blackKing = false;
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            const Piece *cell = board.squares[row][col];
            if (cell == nullptr)
                continue;
            if (cell->symbol == 'K')
            {
                if (cell->color == 0) // WHITE
                    whiteKing = true;
                else
                    blackKing = true;
            }
        }
    }
    return !(whiteKing && blackKing);
}

// Quiescence search to avoid horizon effect
int quiescence(Board &board, int alpha, int beta, int color)
{
    int stand = eval(board);
    if (stand >= beta)
        return beta;
    if (stand > alpha)
        alpha = stand;

    auto caps = generateCaptures(board, color); // Only consider capture moves
    for (auto &move : caps)
    {
        Undo undo;
        applyMove(board, move, undo);
        int score = -quiescence(board, -beta, -alpha, color==0 ? 1 : 0);
        undoMove(board, move, undo);
        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }
    return alpha;
}

// Negamax algorithm with alpha-beta pruning
// Cutting off branches that won't influence the final decision
int negamax(Board &board, int depth, int alpha, int beta, int color)
{
    ++nodesVisited;
    if (depth == 0 || gameOver(board))
        return quiescence(board, alpha, beta, color);

    int best = -INF;
    auto moves = legalMoves(board, color);
    if (moves.empty()) // No legal moves
        return quiescence(board, alpha, beta, color);

    orderMoves(moves);
    for (auto &move : moves)
    {
        Undo undo;
        applyMove(board, move, undo);
        int score = -negamax(board, depth - 1, -beta, -alpha, color==0 ? 1 : 0);
        undoMove(board, move, undo);
        if (score > best)
            best = score;
        if (score > alpha)
            alpha = score;
        if (alpha >= beta)
            break; // beta cutoff
    }
    return best;
}

// API freaking out, set default to 0 - white
int negamax(Board &board, int depth, int alpha, int beta)
{
    return negamax(board, depth, alpha, beta, 0);
}
