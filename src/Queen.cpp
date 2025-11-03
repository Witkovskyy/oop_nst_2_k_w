#include <iostream>
#include "Piece.h"
#include "Board.h"
#include "Queen.h"

Queen::Queen(int c, char s, Position p) : Piece(c, 'Q', p) {}

bool Queen::canMove(Position new_pos, Board* board) {
    Position cur_pos = getPosition();

    int row_diff = new_pos.row - cur_pos.row;
    int col_diff = new_pos.col - cur_pos.col;

    if (new_pos.row < 0 || new_pos.row >= 8 || new_pos.col < 0 || new_pos.col >= 8) return false;
        
    bool diagonal = abs(row_diff) == abs(col_diff);
    bool straight = (row_diff == 0 || col_diff == 0);

    if (!diagonal && !straight)
        return false;

    
    int row_step = (row_diff == 0) ? 0 : (row_diff > 0 ? 1 : -1);
    int col_step = (col_diff == 0) ? 0 : (col_diff > 0 ? 1 : -1);
    int r = cur_pos.row + row_step;
    int c = cur_pos.col + col_step;

    while (r != new_pos.row || c != new_pos.col) {
        if (board->getPieceAt({ r, c }) != nullptr)
            return false;  
        r += row_step;
        c += col_step;
    }

    Piece* target = board->getPieceAt(new_pos);
    if (target == nullptr || target->getColor() != getColor())
        return true;

    return false;
}
