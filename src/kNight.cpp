#include <iostream>
#include "Piece.h"
#include "Board.h"
#include "kNight.h"
Knight::Knight(int c, char s, Position p) : Piece(c, 'N', p) {}

bool Knight::canMove(Position new_pos, Board* board) {
	Position cur_pos = getPosition();
	int row_diff = abs(new_pos.row - cur_pos.row);
	int col_diff = abs(new_pos.col - cur_pos.col);

	if ((row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff ==  2)) {
		Piece* target = board->getPieceAt(new_pos);
		if (target == nullptr || target->getColor() != getColor()) return true;
		return false;
	}
	return false;
}