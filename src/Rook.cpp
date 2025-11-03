#include <iostream>
#include "Piece.h"
#include "Pawn.h"
#include "Board.h"
#include "Rook.h"
using namespace std;
const int SIZEBOARD = 8;


Rook::Rook(int c, char s, Position p) : Piece(c, 'R', p) {}

bool Rook::canMove(Position new_pos, Board* board) {
	Position cur_pos = getPosition();
	int row_diff = new_pos.row - cur_pos.row;
	int col_diff = new_pos.col - cur_pos.col;
	if (new_pos.row >= 8 || new_pos.col >= 8) return false;
	if (new_pos.row < 0 || new_pos.col < 0) return false;

	//wcale nie ruszono
	if (new_pos.row == cur_pos.row && new_pos.col == cur_pos.col)
		return false;


	//porusza sie gora lub dol - zmiana wierszy

	if (row_diff != 0 && col_diff == 0) {
		int step = (row_diff > 0) ? 1 : -1;
		for (int i = cur_pos.row + step; i < new_pos.row; i = i + step) {
			//new_pos.col = cur_pos.col bo nie zmieniamy kolumny w tym przypadku
			if (board->getPieceAt({ i,new_pos.col }) != nullptr) {
				return false;
			}
		}
		Piece* target = board->getPieceAt(new_pos);
		if (target == nullptr || target->getColor() != getColor()) {
			return true;
		}
		return false;
	}

	//porusza sie prawo lub lewo - zmiana kolumn

	if (col_diff != 0 && row_diff == 0) {
		int step = (row_diff > 0) ? 1 : -1;

		
		for (int r = cur_pos.row + step; r != new_pos.row; r += step) {
			if (board->getPieceAt({ r, cur_pos.col }) != nullptr) {
				return false; 
			}
		}
		Piece* target = board->getPieceAt(new_pos);
		if (target == nullptr || target->getColor() != getColor()) {
			return true;
		}

		return false;
	}



	return false;

}