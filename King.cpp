/**
 * @brief Check whether this piece can move to the given square (piece rules only).
 *
 * @details Validates movement pattern for the specific piece type. Additional game rules (e.g., leaving king in check) are handled elsewhere.
 * @param new_pos Board position/index.
 * @param board Board state to operate on.
 * @return True if the condition holds; otherwise false.
 */


﻿#include "Piece.h"
#include "King.h"
#include "Board.h"
#include <cmath> // abs()

using namespace std;

// Constructor
/**
 * @brief Check whether this piece can move to the given square (piece rules only).
 *
 * @details Validates movement pattern for the specific piece type. Additional game rules (e.g., leaving king in check) are handled elsewhere.
 * @param new_pos Board position/index.
 * @param board Board state to operate on.
 * @return True if the condition holds; otherwise false.
 */
King::King(int c, char s, Position p) : Piece(c, 'K', p) {}

bool King::canMove(Position new_pos, Board &board) {
	// Board boundaries and basic movement rules for King
    if (new_pos.row < 0 || new_pos.row >= 8 || new_pos.col < 0 || new_pos.col >= 8)
        return false;

    Position cur_pos = getPosition();
    int row_diff = abs(new_pos.row - cur_pos.row);
    /**
     * @brief Perform `abs`.
     *
     * @details Documentation for `abs`.
     * @param col Board column (file).
     * @return Result of the operation.
     */
    int col_diff = abs(new_pos.col - cur_pos.col);

	// Move geometry: max 1 square in any direction
    if (row_diff > 1 || col_diff > 1)
        return false;

	// Can't stay in the same place
    if (row_diff == 0 && col_diff == 0)
        return false;

	// Collision with friendly piece
    /**
     * @brief Get piece at.
     *
     * @details Documentation for `getPieceAt`.
     * @param new_pos Parameter.
     * @return Requested value.
     */
    Piece* target = board.getPieceAt(new_pos);
    if (target != nullptr && target->getColor() == getColor())
        return false;

    return true;
}