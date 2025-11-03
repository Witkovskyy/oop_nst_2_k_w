#include <iostream>
#include "Piece.h"
#include "Pawn.h"
#include "Board.h"
using namespace std;
const int SIZEBOARD = 8;


Pawn::Pawn(int c, char s, Position p) : Piece(c, 'P', p) {}

bool Pawn::canMove(Position new_pos, Board* board) {
	Position cur_pos = getPosition();
	int row_diff = new_pos.row - cur_pos.row;
	int col_diff = new_pos.col - cur_pos.col;

	int direction = (getColor() == 0) ? 1 : -1;

	//Start pos for pawn from white persepective
	//{1,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7}; WHITE
	//{6,0},{6,1},{6,2},{6,3},{6,4},{6,5},{6,6},{6,7}; BLACK

	//1st check if pawn is on BOARD after move (new_pos)
	if (new_pos.row >= 8 || new_pos.col >= 8) return false;
	if (new_pos.row < 0 || new_pos.col < 0) return false;
	
	if (col_diff == 0 && row_diff == direction)
	{
		if (board->isEmpty(new_pos)) return true;
	}

	//Ruch o dwa z pozycji startowej
	if (col_diff == 0 && row_diff == 2 * direction) {
		if ((getColor() == 0 && cur_pos.row == 1) || (getColor() == 1 && cur_pos.row == 6)) {
			//sprawdzamy pole pomiedzy 
			Position beetween_pos = { cur_pos.row + direction, cur_pos.col };
			if (board->isEmpty(new_pos) && board->isEmpty(beetween_pos)) return true;
		}
	}
	//bicie na ukos
	if (abs(col_diff) == 1 && row_diff == direction) {
		Piece* target = board->getPieceAt(new_pos);
		if (target != nullptr && target->getColor() != getColor()) {
			return true;
		}
	}
	return false;
}


