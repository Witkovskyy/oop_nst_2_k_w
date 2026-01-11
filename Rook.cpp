/**
 * @brief Check whether this piece can move to the given square (piece rules only).
 *
 * @details Validates movement pattern for the specific piece type. Additional game rules (e.g., leaving king in check) are handled elsewhere.
 * @param new_pos Board position/index.
 * @param board Board state to operate on.
 * @return True if the condition holds; otherwise false.
 */


﻿#include "Rook.h"
#include "Board.h"
#include <cmath>
#include <algorithm> // Necessary

/**
 * @brief Check whether this piece can move to the given square (piece rules only).
 *
 * @details Validates movement pattern for the specific piece type. Additional game rules (e.g., leaving king in check) are handled elsewhere.
 * @param new_pos Board position/index.
 * @param board Board state to operate on.
 * @return True if the condition holds; otherwise false.
 */
Rook::Rook(int c, char s, Position p) : Piece(c, 'R', p) {}

bool Rook::canMove(Position new_pos, Board &board) {
    Position cur_pos = getPosition();

	// Boundary check
    if (new_pos.row < 0 || new_pos.row > 7 || new_pos.col < 0 || new_pos.col > 7) return false;

	// Can't stay in the same place
    if (new_pos.row == cur_pos.row && new_pos.col == cur_pos.col) return false;

	// Check if move is vertical or horizontal
    bool isVertical = (new_pos.col == cur_pos.col);
    bool isHorizontal = (new_pos.row == cur_pos.row);

    if (!isVertical && !isHorizontal) return false;

	// Vertical straight move
    if (isVertical) {
        /**
 * @brief Piece method: for.
 *
 * @details Applies to a single chess piece instance.
 * @param r Parameter.
 * @return Result of the operation.
 */
        int start = std::min(cur_pos.row, new_pos.row);
        int end = std::max(cur_pos.row, new_pos.row);

		// Only check squares between start and end
        for (int r = start + 1; r < end; r++) {
            if (board.getPieceAt({ r, cur_pos.col }) != nullptr) {
				return false; // Obstacle found
            }
        }
    }
	// Horizontal straight move
    /**
 * @brief Piece method: if.
 *
 * @details Applies to a single chess piece instance.
 * @param isHorizontal Parameter.
 * @return Result of the operation.
 */
    else if (isHorizontal) {
        int start = std::min(cur_pos.col, new_pos.col);
        int end = std::max(cur_pos.col, new_pos.col);

        for (int c = start + 1; c < end; c++) {
            if (board.getPieceAt({ cur_pos.row, c }) != nullptr) {
				return false; // Obstacle found
            }
        }
    }

    // Target
    /**
     * @brief Get piece at.
     *
     * @details Documentation for `getPieceAt`.
     * @param new_pos Parameter.
     * @return Requested value.
     */
    Piece* target = board.getPieceAt(new_pos);
    if (target == nullptr) return true;
    if (target->getColor() != getColor()) return true;

    return false;
}