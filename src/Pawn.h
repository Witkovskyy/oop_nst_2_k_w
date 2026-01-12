#pragma once
#include <iostream>
#include "Piece.h"
#include "Board.h"
using namespace std;

class Pawn : public Piece {
private:
	int direction;

public:
	/**
	 * @brief Piece method: pawn.
	 *
	 * @details Applies to a single chess piece instance.
	 * @param Position Board position/index.
	 * @return Result of the operation.
	 */
	Pawn(int,char, Position);
	/**
	 * @brief Piece method: canMove.
	 * 
	 * @param pos New position to move to. 
	 * @param board Reference to the board.
	 * @return true If the move is valid.
	 * @return false If the move is not valid.
	 */
	bool canMove(Position pos, Board& board) override;

};



