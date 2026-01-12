/**
 * @file kNight.cpp
 * @brief File implementation for Knight piece.
 * @version 0.1
 * @date 2026-01-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "kNight.h" 
#include "Board.h"
#include <cmath>


Knight::Knight(int c, char s, Position p) : Piece(c, 'N', p) {}
/**
 * @brief Check whether this piece can move to the given square (piece rules only).
 *
 * @details Validates movement pattern for the specific piece type. Additional game rules (e.g., leaving king in check) are handled elsewhere.
 * @param new_pos Board position/index.
 * @param board Board state to operate on.
 * @return True if the condition holds; otherwise false.
 */
bool Knight::canMove(Position new_pos, Board &board) {
    Position cur_pos = getPosition();

	// Check board boundaries
    if (new_pos.row < 0 || new_pos.row > 7 || new_pos.col < 0 || new_pos.col > 7) return false;

    int row_diff = abs(new_pos.row - cur_pos.row);
    /**
     * @brief Perform `abs`.
     *
     * @details Documentation for `abs`.
     * @param col Board column (file).
     * @return Result of the operation.
     */
    int col_diff = abs(new_pos.col - cur_pos.col);

    //  "L" move
    if ((row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff == 2)) {
        Piece* target = board.getPieceAt(new_pos);
        if (target == nullptr || target->getColor() != getColor()) return true;
        return false;
    }
    return false;
}