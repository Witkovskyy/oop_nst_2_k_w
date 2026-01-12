/**
 * @file Queen.h
 * @brief File declaration for Queen piece.
 * @version 0.1
 * @date 2026-01-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once
#include <iostream>
#include "Piece.h"
#include "Board.h"
using namespace std;

/**
 * @brief Class for Queen piece.
 * 
 */
class Queen : public Piece {
private:
	int direction;

public:
	/**
 * @brief Piece method: queen.
 *
 * @details Applies to a single chess piece instance.
 * @param Position Board position/index.
 * @return Result of the operation.
 */
	Queen(int, char, Position);
	bool canMove(Position pos, Board &board);

};

