#pragma once
#include <iostream>
#include <vector>
#include "Piece.h"

const int sizeboard = 8;

class Board {
public:
    Piece* squares[sizeboard][sizeboard];
    Piece* Display();
public:
    Board();
	Board(const Board& other); // Copy constructor
	~Board(); // Destructor

	// New methods for game state checks
	Position findKing(int color);                   // Find king position
	bool isSquareAttacked(Position pos, int enemyColor); // Is the square attacked?
	bool isKingInCheck(int color);                  // Is king in check?
    bool isMoveSafe(Position start, Position end);  // Pins 
	bool isCheckMate(int color);                    // Is checkmate?

    void placePiece(Piece* piece);
    bool isEmpty(Position pos);
    bool movePiece(Position oldpos, Position newpos, Piece* piece);
    void DisplayBoard();
    Piece* getPieceAt(Position pos);
	bool validateMove(Position from, Position to, Piece* piece);
	void promotePawn(Board &board, Position pos, char newSymbol, int color);
	unsigned long long zobristKey = 0;
	void computeZobristHash();
	std::vector<unsigned long long> positionHistory;
};
