/**
 * @brief Check whether this piece can move to the given square (piece rules only).
 *
 * @details Validates movement pattern for the specific piece type. Additional game rules (e.g., leaving king in check) are handled elsewhere.
 * @param pos Board position/index.
 * @param board Board state to operate on.
 * @return True if the condition holds; otherwise false.
 */


﻿#pragma once
#include <iostream>
#include "Piece.h"
#include "Board.h"
using namespace std;

class King : public Piece {
private:
	int direction;

public:
	/**
 * @brief Piece method: king.
 *
 * @details Applies to a single chess piece instance.
 * @param Position Board position/index.
 * @return Result of the operation.
 */
	King(int, char, Position);
	bool canMove(Position pos, Board& board);

};

