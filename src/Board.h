#pragma once
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
	/**
	 * @brief Construct a new Board object
	 * 
	 * @param other Reference to another Board object to copy from.
	 */
	Board(const Board& other); // Copy constructor
	~Board(); // Destructor
	/**
	 * @brief Copy assignment operator
	 * 
	 * @param other Reference to another Board object to copy from.
	 * @return Board& Reference to the current Board object.
	 */
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
	/**
	 * @brief Board operation: check if a square is attacked.
	 * 
	 * @param pos Position to check.
	 * @param enemyColor Color of the attacking side.
	 * @return true If the square is attacked.
	 * @return false If the square is not attacked.
	 */
	bool isSquareAttacked(Position pos, int enemyColor); // Is the square attacked?
	/**
	 * @brief Board operation: check if the king is in check.
	 * 
	 * @param color Color of the king to check.
	 * @return true If the king is in check.
	 * @return false If the king is not in check.
	 */
	bool isKingInCheck(int color);                  // Is king in check?
	/**
	 * @brief Board operation: check if a move is safe.
	 * 
	 * @param start Starting position of the move.
	 * @param end Ending position of the move.
	 * @return true If the move is safe.
	 * @return false If the move is not safe.
	 */
    bool isMoveSafe(Position start, Position end);  // Pins 
	/**
	 * @brief Board operation: check if the game is in checkmate.
	 * 
	 * @param color Color of the side to check.
	 * @return true If the game is in checkmate.
	 * @return false If the game is not in checkmate.
	 */
	bool isCheckMate(int color);                    // Is checkmate?
	/**
	 * @brief Board operation: place a piece on the board.
	 * 
	 * @param piece Pointer to the piece to place.
	 */
    void placePiece(Piece* piece);
    /**
	 * @brief Check whether empty.
	 *
	 * @details Operates on the current board representation and game state.
	 * @param pos Board position/index.
	 * @return True if the condition holds; otherwise false.
	 */
    bool isEmpty(Position pos);
	/**
	 * @brief Board operation: move a piece on the board.
	 * 
	 * @param oldpos Starting position of the piece.
	 * @param newpos New position to move the piece to.
	 * @param piece Pointer to the piece to move.
	 * @return true If the move was successful.
	 * @return false 
	 */
    bool movePiece(Position oldpos, Position newpos, Piece* piece);
	/**
	 * @brief Board operation: display the board.
	 * 
	 */
    void DisplayBoard();
	/**
	 * @brief Get the Piece At object
	 * 
	 * @param pos Board position/index.
	 * @return Piece* Pointer to the piece at the given position.
	 */
    Piece* getPieceAt(Position pos);
	/**
	 * @brief Board operation: validate a move.
	 * 
	 * @param from Starting position of the move.
	 * @param to Ending position of the move.
	 * @param piece Pointer to the piece to move.
	 * @return true If the move is valid.
	 * @return false 
	 */
	bool validateMove(Position from, Position to, Piece* piece);
	/**
	 * @brief Board operation: promote a pawn.
	 * 
	 * @param board Reference to the board.
	 * @param pos Position of the pawn to promote.
	 * @param newSymbol Symbol of the new piece.
	 * @param color Color of the new piece.
	 */
	void promotePawn(Board &board, Position pos, char newSymbol, int color);
	unsigned long long zobristKey = 0;
	/**
	 * @brief Board operation: compute zobrist hash.
	 *
	 * @details Operates on the current board representation and game state.
	 */
	void computeZobristHash();
	/**
	 * @brief Board operation: store position history.
	 * 
	 */
	std::vector<unsigned long long> positionHistory;
};
