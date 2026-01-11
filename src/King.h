#pragma once
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

