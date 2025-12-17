#include <iostream>
#include <cmath> 
#include "Piece.h"
#include "Pawn.h"
#include "Board.h"

using namespace std;
const int SIZEBOARD = 8;

Pawn::Pawn(int c, char s, Position p) : Piece(c, 'P', p) {}

// Zwróć uwagę na & przy board
bool Pawn::canMove(Position new_pos, Board& board) {
	Position cur_pos = getPosition();
	int row_diff = new_pos.row - cur_pos.row;
	int col_diff = new_pos.col - cur_pos.col;

	int direction = (getColor() == 0) ? 1 : -1;

	// 1. Sprawdzenie czy nie wychodzimy poza planszę
	if (new_pos.row >= 8 || new_pos.col >= 8) return false;
	if (new_pos.row < 0 || new_pos.col < 0) return false;

	// 2. Ruch o jedno pole do przodu
	if (col_diff == 0 && row_diff == direction)
	{
		// ZMIANA: board.isEmpty zamiast board->isEmpty
		if (board.isEmpty(new_pos)) return true;
	}

	// 3. Ruch o dwa pola z pozycji startowej
	if (col_diff == 0 && row_diff == 2 * direction) {
		if ((getColor() == 0 && cur_pos.row == 1) || (getColor() == 1 && cur_pos.row == 6)) {
			Position beetween_pos = { cur_pos.row + direction, cur_pos.col };

			// ZMIANA: Używamy kropki (.) bo board jest referencją
			if (board.isEmpty(new_pos) && board.isEmpty(beetween_pos)) return true;
		}
	}

	// 4. Bicie na ukos
	if (abs(col_diff) == 1 && row_diff == direction) {
		// ZMIANA: board.getPieceAt
		Piece* target = board.getPieceAt(new_pos);

		// Sprawdzamy czy cel istnieje i czy jest wrogiem
		if (target != nullptr && target->getColor() != getColor()) {
			return true;
		}
	}

	return false;
}