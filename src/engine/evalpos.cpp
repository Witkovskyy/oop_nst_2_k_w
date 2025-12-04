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

inline int to01(int sign) { return sign > 0 ? 0 : 1; }
inline int toSign(int color01) { return color01 == 0 ? +1 : -1; }

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


static std::vector<Move> legalMoves(Board& board, int color) {
    auto quiet = generateAllMoves(board, color);
    auto caps = generateAllCaptures(board, color);
    quiet.insert(quiet.end(), caps.begin(), caps.end());
    return quiet;
}

static std::vector<Move> generateCaptures(Board &board, int color)
{
    return generateAllCaptures(board, color);
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
    std::sort(moves.begin(), moves.end(), [&](const Move& A, const Move& B) {
        const bool capA = (A.pieceCaptured != nullptr);
        const bool capB = (B.pieceCaptured != nullptr);
        if (capA != capB) return capA > capB;
        if (capA) {
            int sA = 16 * pieceValFromSymbol(A.pieceCaptured->getSymbol()) - pieceValFromSymbol(A.pieceMoved->getSymbol());
            int sB = 16 * pieceValFromSymbol(B.pieceCaptured->getSymbol()) - pieceValFromSymbol(B.pieceMoved->getSymbol());
            if (sA != sB) return sA > sB;
        }
        return false;
        });
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

    auto caps = generateCaptures(board, to01(color)); // Only consider capture moves
    for (auto &move : caps)
    {
        Undo undo;
        applyMove(board, move, undo);
        int score = -quiescence(board, -beta, -alpha, -color);
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
    auto moves = legalMoves(board,to01(color));
    if (moves.empty()) {
        return 0;
    }

    orderMoves(moves);
    for (auto &move : moves)
    {
        Undo undo;
        applyMove(board, move, undo);
        int score = -negamax(board, depth - 1, -beta, -alpha, -color);
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
//int negamax(Board &board, int depth, int alpha, int beta)
//{
//    return negamax(board, depth, alpha, beta);
//}
