#pragma once
#include <iostream>
#include "Piece.h"
#include "Board.h"
using namespace std;

class Rook : public Piece {
private:
	int direction;

public:
	/**
 * @brief Piece method: rook.
 *
 * @details Applies to a single chess piece instance.
 * @param Position Board position/index.
 * @return Result of the operation.
 */
	Rook(int, char, Position);
	bool canMove(Position pos, Board &board);

};
