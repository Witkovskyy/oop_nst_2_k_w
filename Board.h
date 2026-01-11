/**
 * @brief Board operation: display.
 *
 * @details Operates on the current board representation and game state.
 * @return Result of the operation.
 */


﻿#pragma once
#include <iostream>
#include <vector>
#include "Piece.h"

const int sizeboard = 8;

class Board {
public:
    Piece* squares[sizeboard][sizeboard];
    /**
 * @brief Board operation: display.
 *
 * @details Operates on the current board representation and game state.
 * @return Result of the operation.
 */
    Piece* Display();
public:
    Board();
	Board(const Board& other); // Copy constructor
	~Board(); // Destructor
	Board& operator=(const Board& other); // Copy assignment operator

	// New methods for game state checks
	/**
 * @brief Board operation: find king.
 *
 * @details Operates on the current board representation and game state.
 * @param color Side/color parameter.
 * @return Result of the operation.
 */
	Position findKing(int color);                   // Find king position
	bool isSquareAttacked(Position pos, int enemyColor); // Is the square attacked?
	bool isKingInCheck(int color);                  // Is king in check?
    bool isMoveSafe(Position start, Position end);  // Pins 
	bool isCheckMate(int color);                    // Is checkmate?

    void placePiece(Piece* piece);
    /**
 * @brief Check whether empty.
 *
 * @details Operates on the current board representation and game state.
 * @param pos Board position/index.
 * @return True if the condition holds; otherwise false.
 */
    bool isEmpty(Position pos);
    bool movePiece(Position oldpos, Position newpos, Piece* piece);
    void DisplayBoard();
    Piece* getPieceAt(Position pos);
	bool validateMove(Position from, Position to, Piece* piece);
	void promotePawn(Board &board, Position pos, char newSymbol, int color);
	unsigned long long zobristKey = 0;
	/**
 * @brief Board operation: compute zobrist hash.
 *
 * @details Operates on the current board representation and game state.
 */
	void computeZobristHash();
	std::vector<unsigned long long> positionHistory;
};
