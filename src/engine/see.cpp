// Static Exchange Evaluation (SEE)
#include "../Piece.h"
#include "../main.cpp"
#include "val.h"
#include <algorithm>
#include <vector>
#include <limits>

// Minimal Board API expected by this file from main.cpp

static inline int colorOfPiece(Piece *piece)
{
    if (!piece)
        return -1; // none
    return piece->getColor();
}

static bool lineClear(Board &board, Position from, Position to)
{
    int stepRow = (to.row > from.row) - (to.row < from.row);
    int stepCol = (to.col > from.col) - (to.col < from.col);

    int currentRow = from.row + stepRow;
    int currentCol = from.col + stepCol;

    // "walking" towards "to" and ensure all squares are empty
    while (currentRow != to.row || currentCol != to.col)
    {
        Position pos{currentRow, currentCol};
        if (board.getPieceAt(pos) != nullptr)
            return false;
        currentRow += stepRow;
        currentCol += stepCol;
    }
    return true;
}

static bool canAttack(Board &board, Position from, Position to)
{
    if (from.row == to.row && from.col == to.col)
        return false;
    Piece *src = board.getPieceAt({from.row, from.col});
    if (!src)
        return false;
    char symbol = src->getSymbol();
    int srcColor = src->getColor();
    if (symbol == 'P')
    {
        int dir = (srcColor == 0) ? -1 : +1;
        int rr = from.row + dir;
        return (to.row == rr && (to.col == from.col - 1 || to.col == from.col + 1));
    }
    if (symbol == 'N')
    {
        int dr = std::abs(from.row - to.row), dc = std::abs(from.col - to.col);
        return (dr == 1 && dc == 2) || (dr == 2 && dc == 1);
    }
    if (symbol == 'B')
    {
        return std::abs(from.row - to.row) == std::abs(from.col - to.col) && lineClear(board, from, to);
    }
    if (symbol == 'R')
    {
        return (from.row == to.row || from.col == to.col) && lineClear(board, from, to);
    }
    if (symbol == 'Q')
    {
        return ((from.row == to.row || from.col == to.col) || (std::abs(from.row - to.row) == std::abs(from.col - to.col))) && lineClear(board, from, to);
    }
    if (symbol == 'K')
    {
        return std::max(std::abs(from.row - to.row), std::abs(from.col - to.col)) == 1;
    }
    return false;
}

// Returns the cheapest attacker Position of the given color on 'square'. If none, r=-1.
static Position cheapestAttacker(Board &board, Position square, int sideColor)
{
    Position best{-1, -1};
    int bestVal = 1e9;
    for (int row = 0; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
        {
            Piece *piece = board.getPieceAt({row, col});
            if (!piece)
                continue;
            if (piece->getColor() != sideColor)
                continue;
            if (!canAttack(board, {row, col}, square))
                continue;
            int val = pieceValFromSymbol(piece->getSymbol());
            if (val < bestVal)
            {
                bestVal = val;
                best = {row, col};
            }
        }
    return best;
}

// SEE: positive result = gain for 'sideToMove' initiating the exchange on 'target'.
int see(Board &start, Position target, int sideToMove)
{
    Piece *victim = start.getPieceAt({target.row, target.col});
    if (!victim)
        return 0;

    Board boardCopy = start; // uses Board copy-constructor if available

    int gain[32];
    int depth = 0;
    gain[0] = pieceValFromSymbol(victim->getSymbol());

    int stm = sideToMove;

    while (true)
    {
        Position getAttacker = cheapestAttacker(boardCopy, target, stm);
        if (getAttacker.row == -1)
            break;

        Piece *attacker = boardCopy.getPieceAt({getAttacker.row, getAttacker.col});
        int attackerVal = pieceValFromSymbol(attacker->getSymbol());

        int next = attackerVal - gain[depth];
        gain[++depth] = next;

        // perform static capture on board copy
        // remove attacker from its Position
        boardCopy.movePiece({getAttacker.row, getAttacker.col}, {target.row, target.col}, attacker);

        // switch side
        sideToMove = (sideToMove == 0) ? 1 : 0;
    }

    for (int i = depth - 1; i >= 0; --i)
        gain[i] = std::max(-gain[i + 1], gain[i]);

    return gain[0];
}
