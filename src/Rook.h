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
	/**
	 * @brief Abstract method to check if a move is valid.
	 * 
	 * @param pos New position to move to.
	 * @param board Reference to the board.
	 * @return true If the move is valid.
	 * @return false If the move is not valid.
	 */
	bool canMove(Position pos, Board &board);

};
