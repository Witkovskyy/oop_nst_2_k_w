#include "Piece.h"
#include "King.h"
#include "Board.h"
#include <cmath>   // dla abs()
using namespace std;

const int SIZEBOARD = 8;

King::King(int c, char s, Position p) : Piece(c, 'K', p) {} // 'K' zamiast 'B' dla króla

bool King::canMove(Position new_pos, Board* board) {
    Position cur_pos = getPosition();
    int row_diff = abs(new_pos.row - cur_pos.row);
    int col_diff = abs(new_pos.col - cur_pos.col);

    if (new_pos.row < 0 || new_pos.row >= SIZEBOARD || new_pos.col < 0 || new_pos.col >= SIZEBOARD)
        return false;

    if (row_diff > 1 || col_diff > 1)
        return false;

    Piece* target = board->getPieceAt(new_pos);
    if (target != nullptr && target->getColor() == getColor())
        return false;


    return true;
}
