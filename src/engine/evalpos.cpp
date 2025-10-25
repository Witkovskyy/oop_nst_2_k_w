#pragma once
#include "../main.cpp"
#include <algorithm>
#include <vector>
#include <limits>

// Simple instrumentation: count nodes visited by negamax
static long nodesVisited = 0;
long get_nodes_visited() { return nodesVisited; }

static const int INF = std::numeric_limits<int>::max();

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
            int value = static_cast<int>(cell->value);
            if (cell->color == 0) // Assuming 0 represents WHITE
                score += value;
            else
                score -= value;
        }
    }
    return score;
}

// Declaration of helper functions
//To fill in later


static std::vector<Position> legalMoves(const Board &board)
{
    return {};
}
// Helper returns all legal moves as a fallback implementation
static std::vector<Position> generateCaptures(Board &board)
{
    // To replace
    return legalMoves(board);
}

static void undoMove(Board &board, Position oldpos, Undo &undo)
{
    board.squares[oldpos.row][oldpos.col] = undo.pieceMoved;
    board.squares[undo.to.row][undo.to.col] = undo.pieceCaptured;
}

static void applyMove(Board &board, Position oldpos, Undo &undo)
{
    board.squares[undo.to.row][undo.to.col] = undo.pieceMoved;
    board.squares[oldpos.row][oldpos.col] = nullptr;
}

static void orderMoves(std::vector<Position> &moves)
{
    // no-op for now
    (void)moves;
}

// Simple check: game over if one king is missing
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

struct Undo
{
    Position from;
    Position to;
    Piece *pieceMoved;
    Piece *pieceCaptured;
};


// Quiescence search to avoid horizon effect
int quiescence(Board &board, int alpha, int beta)
{
    int stand = eval(board);
    if (stand >= beta)
        return beta;
    if (stand > alpha)
        alpha = stand;

    auto caps = generateCaptures(board); // Only consider capture moves
    for (auto &move : caps)
    {
        Undo undo;
        applyMove(board, move, undo);
        int score = -quiescence(board, -beta, -alpha);
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
int negamax(Board &board, int depth, int alpha, int beta)
{
    ++nodesVisited;
    if (depth == 0 || gameOver(board))
        return quiescence(board, alpha, beta);

    int best = -INF;
    auto moves = legalMoves(board);
    if (moves.empty()) // No legal moves
        return quiescence(board, alpha, beta);

    orderMoves(moves);
    for (auto &move : moves)
    {
        Undo undo;
        applyMove(board, move, undo);
        int score = -negamax(board, depth - 1, -beta, -alpha);
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
