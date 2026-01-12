/**
 * @file Pawn.cpp
 * @brief File implementation for Pawn piece.
 * @version 0.1
 * @date 2026-01-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <iostream>
#include <cmath> 
#include "Piece.h"
#include "Pawn.h"
#include "Board.h"

using namespace std;
const int SIZEBOARD = 8;


Pawn::Pawn(int c, char s, Position p) : Piece(c, 'P', p) {}
/**
 * @brief Check whether this piece can move to the given square (piece rules only).
 *
 * @details Validates movement pattern for the specific piece type. Additional game rules (e.g., leaving king in check) are handled elsewhere.
 * @param new_pos Board position/index.
 * @param board Board state to operate on.
 * @return True if the condition holds; otherwise false.
 */
bool Pawn::canMove(Position new_pos, Board& board) {
	Position cur_pos = getPosition();
	int row_diff = new_pos.row - cur_pos.row;
	int col_diff = new_pos.col - cur_pos.col;

	int direction = (getColor() == 0) ? 1 : -1;

	// Check board boundaries
	if (new_pos.row >= 8 || new_pos.col >= 8) return false;
	if (new_pos.row < 0 || new_pos.col < 0) return false;

	// Move one forward
	if (col_diff == 0 && row_diff == direction)
	{
		if (board.isEmpty(new_pos)) return true;
	}

	// Move two forward from starting position
	if (col_diff == 0 && row_diff == 2 * direction) {
		if ((getColor() == 0 && cur_pos.row == 1) || (getColor() == 1 && cur_pos.row == 6)) {
			Position beetween_pos = { cur_pos.row + direction, cur_pos.col };

			if (board.isEmpty(new_pos) && board.isEmpty(beetween_pos)) return true;
		}
	}

	// Diagonal capture
	if (abs(col_diff) == 1 && row_diff == direction) {
		/**
		 * @brief Get piece at.
		 *
		 * @details Documentation for `getPieceAt`.
		 * @param new_pos Parameter.
		 * @return Requested value.
		 */
		Piece* target = board.getPieceAt(new_pos);

		// Check if there is an opponent piece to capture
		if (target != nullptr && target->getColor() != getColor()) {
			return true;
		}
	}

	return false;
}