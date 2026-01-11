#pragma once
#include <iostream>
#include "Bishop.h"
#include "Board.h"
using namespace std;

class Bishop : public Piece {
private:
	int direction;

public:
	/**
 * @brief Piece method: bishop.
 *
 * @details Applies to a single chess piece instance.
 * @param Position Board position/index.
 * @return Result of the operation.
 */
	Bishop(int, char, Position);
	bool canMove(Position pos, Board &board);

};

