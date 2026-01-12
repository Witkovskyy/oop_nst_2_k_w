/**
 * @file King.h
 * @brief File declaration for King piece.
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
 * @brief Class for King piece.
 * 
 */
class King : public Piece {
private:
	int direction;

public:
	/**
 * @brief Piece method: king.
 * @details Applies to a single chess piece instance.
 * @param Position Board position/index.
 * @return Result of the operation.
 */
	King(int, char, Position);
	/**
	 * @brief Piece method: canMove.
	 * 
	 * @param pos New position to move to. 
	 * @param board Reference to the board.
	 * @return true If the move is valid.
	 * @return false If the move is not valid.
	 */
	bool canMove(Position pos, Board& board);

};

