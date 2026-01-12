#pragma once
#include <iostream>
#include "Piece.h"
#include "Board.h"
using namespace std;

class Knight : public Piece {
private:
	int direction;

public:
/**
 * @brief Construct a new Knight object
 * @param int Color of the knight.
 * @param char Symbol of the knight.
 * @param Position Initial position of the knight.
 * 
 */
	Knight(int, char, Position);
	/**
	 * @brief Piece method: canMove.
	 * 
	 * @param pos New position to move to. 
	 * @param board Reference to the board.
	 * @return true If the move is valid.
	 * @return false If the move is not valid.
	 */
	bool canMove(Position pos, Board &board);

};

